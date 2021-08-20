#include "net/gcoap.h"
#include "msg.h"
#include "shell.h"
#include "evtimer.h"
#include "evtimer_msg.h"
#include "random.h"
#include "net/gnrc/netif.h"
#include <arpa/inet.h>
#include "net/nanocoap/cache.h"

#ifndef IPV6_PREFIX
#define IPV6_PREFIX         { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0 }
#endif
#ifndef IPV6_PREFIX_LEN
#define IPV6_PREFIX_LEN     (64U)
#endif

gnrc_netif_t *mynetif;
uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
char hwaddr_str[GNRC_NETIF_L2ADDR_MAXLEN * 3];
extern unsigned reqtx, resprx;

static const ipv6_addr_t _ipv6_prefix = { .u8 = IPV6_PREFIX };

ipv6_addr_t _my_link_local, _my_global;
uint16_t _my_id;
char _my_link_local_str[IPV6_ADDR_MAX_STR_LEN];
char _my_global_str[IPV6_ADDR_MAX_STR_LEN];
static bool gw_node = false;

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern gcoap_listener_t app_listener;
extern void send_static_request(uint16_t nodeid);
extern void send_static_request_proxy(uint16_t nodeid);
extern void install_routes(char *laddr, char *toaddr_str, char *nhaddr_str);

static const uint16_t narr[] = NARR;
static const char* gwarr[] = GWARR;
static const size_t narr_sz = ARRAY_SIZE(narr);
static evtimer_t evtimer;
static evtimer_msg_event_t events[ARRAY_SIZE(narr)];

//#define EVENT_TIME (1250 + random_uint32_range(0, 50))
//#define EVENT_TIME (1500 + random_uint32_range(0, 1000))
#define EVENT_TIME (2000 + random_uint32_range(0, 1000))
//#define EVENT_TIME (15000 + random_uint32_range(0, 5000))
//#define EVENT_TIME (1000 + random_uint32_range(0, 500))
//#define EVENT_TIME (900 + random_uint32_range(0, 200))

void get_addr(uint16_t id, char *address)
{
    (void) id;
    (void) address;
#define MYMAP(ID,ADDR)                                          \
    if (id == ID) {                                             \
        memcpy(address, ADDR, strlen(ADDR)+1);                  \
        return;                                                 \
    }
#include "idaddr.inc"
#undef MYMAP
}

uint16_t get_id(char *address)
{
    (void) address;
#define MYMAP(ID,ADDR)                                           \
    if (!memcmp(ADDR, address, strlen(ADDR))) {                  \
        return ID;                                               \
    }
#include "idaddr.inc"
#undef MYMAP
    return 0;
}

int get_proxy_nexthop(char *dest, char *address)
{
    (void) dest;
    (void) address;
#define ROUTE(myid, laddr, toaddr, nhaddr)                              \
    if (_my_id == myid && !strcmp(dest, toaddr)) {                      \
        memcpy(address, nhaddr, strlen(nhaddr)+1);                      \
        return !strcmp(dest+8, nhaddr+4);                               \
    }
#include "routesdown.inc"
#undef ROUTE
    return 0;
}

typedef struct {
    int in_use;
    sock_udp_ep_t ep;
#if IS_ACTIVE(MODULE_NANOCOAP_CACHE)
    uint8_t cache_key[CONFIG_NANOCOAP_CACHE_KEY_LENGTH];
#endif
} client_ep_t;
static sock_udp_ep_t fwd_remote = { .family = AF_INET6, .port = COAP_PORT,};
ssize_t forward_to_forwarders(coap_pkt_t *client_pkt,
                              client_ep_t *client_ep,
                              sock_udp_ep_t *origin_server_ep,
                              gcoap_resp_handler_t resp_handler)
{
    (void) client_pkt;
    (void) client_ep;
    (void) origin_server_ep;
    char addr[IPV6_ADDR_MAX_STR_LEN];
    char nexthop[IPV6_ADDR_MAX_STR_LEN];
    ipv6_addr_t nexthop_addr;
    size_t len = client_pkt->payload_len + (client_pkt->payload - (uint8_t *)client_pkt->hdr);

    inet_ntop(client_ep->ep.family, origin_server_ep->addr.ipv6, addr, INET6_ADDRSTRLEN);

    int lastforwarder = get_proxy_nexthop(addr, nexthop);
    if (lastforwarder) {
        return 0;
    }
    ipv6_addr_from_str(&nexthop_addr, nexthop);
    memcpy(&fwd_remote.addr.ipv6[0], &nexthop_addr.u8[0], sizeof(nexthop_addr.u8));
    fwd_remote.netif = (uint16_t) gnrc_netif_iter(NULL)->pid;

    len = gcoap_req_send((uint8_t *)client_pkt->hdr, len,
                         &fwd_remote,
                         resp_handler, (void *)client_ep);
    return len;
}

static int _send_get(int argc, char **argv) {
    (void)argc;
    (void)argv;
    evtimer_init_msg(&evtimer);
    for (unsigned i = 0; i < narr_sz; i++) {
        events[i].event.offset = (i+1) * 500;
        events[i].msg.content.value = i;
        evtimer_add_msg(&evtimer, &events[i], sched_active_pid);
    }
    unsigned i = 0;
    while ((i++) < 100 * narr_sz) {
        msg_t m;
        msg_receive(&m);
#if EXP_CONFIG_PROXY
        send_static_request_proxy(narr[m.content.value]);
#else
        send_static_request(narr[m.content.value]);
#endif
        events[m.content.value].event.offset = EVENT_TIME;
        evtimer_add_msg(&evtimer, &events[m.content.value], sched_active_pid);
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "send", "", _send_get },
    { NULL, NULL, NULL }
};

int main(void)
{
    gcoap_register_listener(&app_listener);

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* find first netif */
    mynetif = gnrc_netif_iter(NULL);

    uint16_t src_len = 8U;
    gnrc_netapi_set(mynetif->pid, NETOPT_SRC_LEN, 0, &src_len, sizeof(src_len));
#ifdef BOARD_NATIVE
    gnrc_netapi_get(mynetif->pid, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
#else
    gnrc_netapi_get(mynetif->pid, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));
#endif
    gnrc_netif_addr_to_str(hwaddr, sizeof(hwaddr), hwaddr_str);

    /* get first ipv6 address from netif */
    gnrc_netif_ipv6_addrs_get(mynetif, &_my_link_local, sizeof(_my_link_local));
    ipv6_addr_to_str(_my_link_local_str, &_my_link_local, sizeof(_my_link_local_str));

    /* set global ipv6 address */
    memcpy(&_my_global, &_my_link_local, sizeof(_my_global));
    ipv6_addr_init_prefix(&_my_global, &_ipv6_prefix, IPV6_PREFIX_LEN);
    ipv6_addr_to_str(_my_global_str, &_my_global, sizeof(_my_global_str));
    gnrc_netif_ipv6_addr_add(mynetif, &_my_global, 128, GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);

    netopt_enable_t opt = NETOPT_DISABLE;
    opt = NETOPT_DISABLE;
    gnrc_netapi_set(mynetif->pid, NETOPT_ACK_REQ, 0, &opt, sizeof(opt));

    for (unsigned i = 0; i < sizeof(gwarr) / sizeof(gwarr[0]); i++) {
        if (!strcmp(gwarr[i], _my_global_str)) {
            gw_node=true;
        }
    }

    _my_id = get_id(_my_global_str);

    printf("addr;%u;%s;%s;%s;%u\n", _my_id, hwaddr_str, _my_link_local_str, _my_global_str, gw_node);

#define ROUTE(myid, laddr, toaddr, nhaddr) install_routes(laddr, toaddr, nhaddr);
#include "routesdown.inc"
#include "routesup.inc"
#undef ROUTE

    random_init(*((uint32_t *)hwaddr));

    if (gw_node) {
        puts("");
        xtimer_sleep(15);
        puts("start");
        _send_get(0, NULL);
        xtimer_sleep(10);
        printf("end;%u;%u\n", reqtx, resprx);
    }

    shell_run(shell_commands, line_buf, sizeof(line_buf));

    return 0;
}
