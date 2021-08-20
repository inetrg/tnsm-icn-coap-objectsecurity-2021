#include <stdio.h>

#include "msg.h"
#include "shell.h"
#include "ccn-lite-riot.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/pktdump.h"
#include "ccnl-callbacks.h"
#include "ccnl-producer.h"
#include "ccnl-ext-hmac.h"
#include "random.h"

/* main thread's message queue */
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

gnrc_netif_t *mynetif;
uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
char hwaddr_str[GNRC_NETIF_L2ADDR_MAXLEN * 3];
unsigned reqtx, resprx;
extern unsigned long rreqtx;

static unsigned char int_buf[CCNL_MAX_PACKET_SIZE];
static unsigned char data_buf[CCNL_MAX_PACKET_SIZE];

extern void install_routes(char *laddr, char *toaddr, char *nhaddr);

uint16_t _my_id;
static const uint16_t narr[] = NARR;
static const size_t narr_sz = ARRAY_SIZE(narr);
static const char* gwarr[] = GWARR;
static evtimer_t evtimer;
static evtimer_msg_event_t events[ARRAY_SIZE(narr)];
static bool gw_node = false;

extern struct ccnl_relay_s ccnl_relay;

#if NDN_SEC
#include "crypto.h"
static uint8_t hmac_key[64] = { 0 };
uint8_t nonce[13] = { 0 };
uint8_t aes_key[16] = "secretPSK";
#endif

#define EVENT_TIME (2000 + random_uint32_range(0, 1000))
//#define EVENT_TIME (1000 + random_uint32_range(0, 500))

void get_addr(uint16_t id, char *address)
{
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
#define MYMAP(ID,ADDR)                                           \
    if (!memcmp(ADDR, address, strlen(ADDR))) {                  \
        return ID;                                               \
    }
#include "idaddr.inc"
#undef MYMAP
    return 0;
}

int my_app_RX(struct ccnl_relay_s *ccnl, struct ccnl_content_s *c)
{
    (void) ccnl;
    char s[CCNL_MAX_PREFIX_SIZE];
    (void) s;

#if NDN_SEC
    static uint8_t md[32];
    static size_t mdlen = sizeof(md);

    ccnl_hmac256_sign(hmac_key, sizeof(hmac_key), c->pkt->buf->data, c->pkt->buf->datalen - 34, md, &mdlen);

    uint16_t temp;
    dtls_decrypt((const unsigned char *)c->pkt->content, 10, (unsigned char *)&temp, nonce, aes_key, sizeof(aes_key), NULL, 0);
#endif

    ccnl_prefix_to_str(c->pkt->pfx, s, CCNL_MAX_PREFIX_SIZE);

    printf("rprx;%.*s\n", 5, s+19);
    resprx++;

    return 0;
}


static void send_static_request(uint16_t nodeid)
{
    char req_uri[32];
    struct ccnl_prefix_s *prefix = NULL;
    static unsigned long i = 0;

    reqtx++;

    memset(int_buf, 0, CCNL_MAX_PACKET_SIZE);
    char addrstr[48];
    get_addr(nodeid, addrstr);
    unsigned group = (unsigned) random_uint32_range(0,10);
    snprintf(req_uri, 32, "/temperature/%.*s/%u", 5, addrstr, group);
    prefix = ccnl_URItoPrefix(req_uri, CCNL_SUITE_NDNTLV, NULL);

    ccnl_send_interest(prefix, int_buf, CCNL_MAX_PACKET_SIZE, NULL);
    ccnl_prefix_free(prefix);

    printf("rqtx;%05lu;%u\n", i-1, nodeid);
}

static int _send_get(int argc, char **argv)
{
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
        send_static_request(narr[m.content.value]);
        events[m.content.value].event.offset = EVENT_TIME;
        evtimer_add_msg(&evtimer, &events[m.content.value], sched_active_pid);
    }
    return 0;
}

static struct ccnl_content_s *sensor_cont_and_cache(struct ccnl_relay_s *relay, struct ccnl_pkt_s *pkt)
{
    (void) pkt;
    (void) relay;
    size_t offs = CCNL_MAX_PACKET_SIZE;

//    char buffer[33];
//    size_t len = sprintf(buffer, "%s", "{\"id\":\"0x12a77af232\",\"val\":\"on\"}");
//    buffer[len]='\0';

    size_t reslen = 0;

#if NDN_SEC
    uint16_t temp_raw = 2124;
    uint8_t temp[16];
    uint16_t keyid = 0;
    int aeslen = dtls_encrypt((const unsigned char *)&temp_raw, sizeof(temp_raw), temp, nonce, aes_key, sizeof(aes_key), NULL, 0);

    ccnl_ndntlv_prependSignedContent(pkt->pfx, (unsigned char*) &temp, aeslen, NULL, NULL, hmac_key, (uint8_t *)&keyid, sizeof(keyid), &offs, data_buf, &reslen);
#else
    uint16_t temp = 2124;

    ccnl_ndntlv_prependContent(pkt->pfx, (unsigned char*) &temp, sizeof(temp), NULL, NULL, &offs, data_buf, &reslen);
#endif

    size_t len = sizeof(temp);

    unsigned char *olddata;
    unsigned char *data = olddata = data_buf + offs;

    uint64_t typ;

    if (ccnl_ndntlv_dehead(&data, &reslen, &typ, &len) || typ != NDN_TLV_Data) {
        puts("ERROR in producer_func");
        return 0;
    }

    struct ccnl_content_s *c = 0;
    struct ccnl_pkt_s *pk = ccnl_ndntlv_bytes2pkt(typ, olddata, &data, &reslen);
    //puts("CREATE CONTENT");
    c = ccnl_content_new(&pk);
    if (c) {
        //c->flags |= CCNL_CONTENT_FLAGS_STATIC;
        ccnl_content_add2cache(relay, c);
        char s[CCNL_MAX_PREFIX_SIZE];
        ccnl_prefix_to_str(c->pkt->pfx, s, CCNL_MAX_PREFIX_SIZE);
        printf("rptx;%.*s\n", 5, s+19);
    }

    return c;
}

struct ccnl_content_s *sensor_producer_func(struct ccnl_relay_s *relay,
                                            struct ccnl_face_s *from,
                                            struct ccnl_pkt_s *pkt) {
    (void) relay;
    (void) from;

    if(pkt->pfx->compcnt == 3) { /* /hwaddr/temperature/<value> */
        if (!memcmp(pkt->pfx->comp[1], hwaddr_str, 5) &&
            !memcmp(pkt->pfx->comp[0], "temperature", pkt->pfx->complen[0])) {
            return sensor_cont_and_cache(relay, pkt);
        }
    }
    return NULL;
}

static const shell_command_t shell_commands[] = {
    { "send", "", _send_get },
    { NULL, NULL, NULL }
};

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    ccnl_core_init();

    ccnl_start();

    mynetif = gnrc_netif_iter(NULL);
    ccnl_open_netif(mynetif->pid, GNRC_NETTYPE_CCN);

    uint16_t src_len = 8U;
    gnrc_netapi_set(mynetif->pid, NETOPT_SRC_LEN, 0, &src_len, sizeof(src_len));
#ifdef BOARD_NATIVE
    gnrc_netapi_get(mynetif->pid, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
#else
    gnrc_netapi_get(mynetif->pid, NETOPT_ADDRESS_LONG, 0, hwaddr, sizeof(hwaddr));
#endif
    gnrc_netif_addr_to_str(hwaddr, sizeof(hwaddr), hwaddr_str);

    netopt_enable_t opt = NETOPT_DISABLE;
    opt = NETOPT_DISABLE;
    gnrc_netapi_set(mynetif->pid, NETOPT_ACK_REQ, 0, &opt, sizeof(opt));

#if 0
#ifdef MODULE_NETIF
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_CCN_CHUNK, &dump);
#endif
#endif

    for (unsigned i = 0; i < sizeof(gwarr) / sizeof(gwarr[0]); i++) {
        if (!strcmp(gwarr[i], hwaddr_str)) {
            gw_node=true;
        }
    }

    _my_id = get_id(hwaddr_str);

    printf("addr;%u;%s;%s;%s;%u\n", _my_id, hwaddr_str, hwaddr_str, hwaddr_str, gw_node);

    bool leaf = false;
    for (unsigned i = 0; i < narr_sz; i++) {
        if (narr[i] == _my_id) {
            leaf = true;
            break;
        }
    }

    if (leaf) {
        ccnl_set_local_producer(sensor_producer_func);
    }

#define ROUTE(myid, laddr, toaddr, nhaddr) install_routes(laddr, toaddr, nhaddr);
#include "routesdown.inc"
#undef ROUTE

    random_init(*((uint32_t *)hwaddr));

    if (gw_node) {
        puts("");
        xtimer_sleep(15);
        puts("start");
        ccnl_relay.max_cache_entries = 0;
        _send_get(0, NULL);
        xtimer_sleep(10);
        printf("end;%u;%u\n", reqtx, resprx);
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
