#include <net/gcoap.h>
#include "od.h"
#include "random.h"

unsigned reqtx = 0, resprx = 0;
unsigned long rreqtx;

static sock_udp_ep_t remote = { .family = AF_INET6, .port = COAP_PORT,};

extern uint16_t _my_id;
extern void get_addr(uint16_t id, char *address);
extern int get_proxy_nexthop(char *dest, char *address);

static void handle_static_response(const struct gcoap_request_memo *memo, coap_pkt_t *pdu, const sock_udp_ep_t *remote)
{
    (void)remote;
    uint16_t msgid = ntohs(pdu->hdr->id);

    if (memo->state != GCOAP_MEMO_RESP) {
        printf("error;rp;%u\n", msgid);
        return;
    }

    printf("rprx;%u\n", msgid);
    resprx++;
    return;
}

void send_static_request_proxy(uint16_t nodeid) __attribute__ ((unused));
void send_static_request_proxy(uint16_t nodeid) {
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    ipv6_addr_t addr;
    uint16_t msgid;
    ssize_t bytes;
    (void) msgid;

    reqtx++;

    remote.netif = (uint16_t) gnrc_netif_iter(NULL)->pid;

    /* parse destination address */
    char addrstr[48];
    char nexthop[64];
    char proxy_uri[66];
    int len = 0;

    get_addr(nodeid, addrstr);
    get_proxy_nexthop(addrstr, nexthop);
    ipv6_addr_from_str(&addr, nexthop);

    unsigned group = (unsigned) random_uint32_range(0,10);
    snprintf(proxy_uri, 66, "coap://[%s]:%u/temperature%u", addrstr, 5683, group);
    gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET, NULL);
    coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
    coap_opt_add_proxy_uri(&pdu, proxy_uri);
    len = coap_opt_finish(&pdu, COAP_OPT_FINISH_NONE);

    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));
    bytes = gcoap_req_send(buf, len, &remote, handle_static_response, NULL);

    if (bytes <= 0) {
        printf("error;rqs\n");
        return;
    }

    msgid = ntohs(pdu.hdr->id);
    printf("rqtx;%u;%u;%u\n", msgid, nodeid, group);
    return;
}

void send_static_request(uint16_t nodeid) __attribute__ ((unused));
void send_static_request(uint16_t nodeid) {
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    ipv6_addr_t addr;
    uint16_t msgid;
    ssize_t bytes;
    (void) msgid;

    reqtx++;

    remote.netif = (uint16_t) gnrc_netif_iter(NULL)->pid;

    /* parse destination address */
    char addrstr[48];

    get_addr(nodeid, addrstr);
    ipv6_addr_from_str(&addr, addrstr);

    char uri[32];
    unsigned group = (unsigned) random_uint32_range(0,10);
    snprintf(uri, 32, "/temperature%u", (unsigned) group);
    bytes = gcoap_request(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET, uri);
    if (bytes < 0) {
        printf("error;rqb\n");
        return;
    }

#if EXP_CONFIG_CON
    coap_hdr_set_type(pdu.hdr, COAP_TYPE_CON);
#else
    coap_hdr_set_type(pdu.hdr, COAP_TYPE_NON);
#endif
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));
    bytes = gcoap_req_send(buf, pdu.payload - (uint8_t*)pdu.hdr + pdu.payload_len, &remote, handle_static_response, NULL);
    msgid = ntohs(pdu.hdr->id);

    if (bytes <= 0) {
        printf("error;rqs\n");
        return;
    }

    printf("rqtx;%u;%u;%u\n", msgid, nodeid, group);
    return;
}

static ssize_t app_coap(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;
    uint16_t msgid = ntohs(pdu->hdr->id);

    printf("rqrx;%u\n", msgid);
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
    msgid = ntohs(pdu->hdr->id);

    uint16_t temp = 2124;

    printf("rptx;%u\n", msgid);
    return resp_len + sizeof(temp);
}

static const coap_resource_t _resources[] = {
    { "/temperature0", COAP_GET, app_coap, NULL },
    { "/temperature1", COAP_GET, app_coap, NULL },
    { "/temperature2", COAP_GET, app_coap, NULL },
    { "/temperature3", COAP_GET, app_coap, NULL },
    { "/temperature4", COAP_GET, app_coap, NULL },
    { "/temperature5", COAP_GET, app_coap, NULL },
    { "/temperature6", COAP_GET, app_coap, NULL },
    { "/temperature7", COAP_GET, app_coap, NULL },
    { "/temperature8", COAP_GET, app_coap, NULL },
    { "/temperature9", COAP_GET, app_coap, NULL },
};

gcoap_listener_t app_listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    NULL,
    NULL,
    NULL
};
