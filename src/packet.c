#include "packet.h"
#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */
#include <stdio.h>  /* ssize_t */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <zlib.h>


/* Extra #includes */
/* Your code will be inserted here */

struct __attribute__((__packed__)) pkt {
    /* Your code will be inserted here */
    uint8_t type : 2; // 2 bits
    uint8_t tr : 1; // 1 bits
    uint8_t window : 5; // 5 bits
    uint16_t length : 16; // 16 bits
    uint8_t seqnum : 8; // 8 bits
    uint32_t timestamp : 32; // 32 bits
    uint32_t crc1 : 32; // 32 bits
    char* payload; // 512 bytes max
    uint32_t crc2 : 32; // 32 bits
};

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    return (pkt_t*) malloc(sizeof(pkt_t));
    /* Your code will be inserted here */
}

void pkt_del(pkt_t *pkt)
{
    if (pkt != NULL)
    {
        if (pkt->payload != NULL)
        {
            free(pkt->payload);
        }
        free(pkt);
    }

}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    if (pkt == NULL || len < 1)
    {
        ERROR("E_UNCONSISTENT");
        return E_UNCONSISTENT;
    }
    uint8_t type = (data[0] >> 6) &  0x03;
    if ((len < 10 && (type == PTYPE_ACK || type == PTYPE_NACK)) || (len < 12 && type == PTYPE_DATA))
    {
        ERROR("E_NOHEADER");
        return E_NOHEADER;
    }

    if (pkt_set_type(pkt, type) == E_TYPE) {
        ERROR("E_TYPE");
        return E_TYPE;
    }

    uint8_t tr = (data[0] & 0x20) >> 5; // 0b00100000
    if (tr && type != PTYPE_DATA) // Paque tronqué
    {
        ERROR("E_UNCONSISTENT");
        return E_UNCONSISTENT;
    }
    if (pkt_set_tr(pkt, tr) == E_TR) return E_TR;

    uint8_t window = data[0] & 0x1F; // 0b00011111
    if (pkt_set_window(pkt, window) ==  E_WINDOW)
    {
        ERROR("E_WINDOW");
        return E_WINDOW;
    }

    uint32_t timestamp, crc1;
    if (type == PTYPE_DATA)
    {
        uint16_t lengthN;
        memcpy(&lengthN, data+1, 2);
        uint16_t lengthH = ntohs(lengthN);
        if (16 + (size_t) lengthH != len)
        {
            ERROR("E_LENGTH");
            return E_LENGTH;
        }
        if (pkt_set_length(pkt, lengthH) == E_LENGTH) return E_LENGTH;
        uint8_t seqnum = data[3];
        pkt_set_seqnum(pkt, seqnum);
        // FIN DU HEADER

        memcpy(&timestamp, data+4, 4);
        pkt_set_timestamp(pkt, timestamp);

        memcpy(&crc1, data+8, 4);
        crc1 = ntohl(crc1);
        uint32_t crcCheck = crc32(0L, Z_NULL, 0);
        crcCheck = crc32(crcCheck, (Bytef *) data, 8);
        if (crcCheck != crc1)
        {
            ERROR("E_CRC1");
            return E_CRC;
        }
        pkt_set_crc1(pkt, crc1);

        if (lengthH)
        {
            uint32_t crcCheckP = crc32(0L, Z_NULL, 0);
            crcCheckP = crc32(crcCheckP, (Bytef *) (data+12), lengthH);
            uint32_t crc2;
            memcpy(&crc2, data + 12 + lengthH, 4);
            crc2 = ntohl(crc2);
            if (crcCheckP != crc2) {
                ERROR("E_CRC2");
                return E_CRC;
            }
            pkt_set_payload(pkt, data+12, lengthH);
        }
    }

    else {
        uint8_t seqnum = data[1];
        pkt_set_seqnum(pkt, seqnum);
        // FIN DU HEADER

        memcpy(&timestamp, data+2, 4);
        pkt_set_timestamp(pkt, timestamp);
        memcpy(&crc1, data+6, 4);
        crc1 = ntohl(crc1);
        uint32_t crcCheck = crc32(0L, Z_NULL, 0);
        crcCheck = crc32(crcCheck, (Bytef *) data, 6);
        if (crcCheck != crc1) {
            ERROR("E_CRC1");
            return E_CRC;
        }
        pkt_set_crc1(pkt, crc1);
    }

    return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    /* Your code will be inserted here */
    if(!pkt_get_tr(pkt) && (size_t)*len < (size_t) (pkt_get_length(pkt) + 16)) {
        return E_NOMEM;
    }

    // Premier byte
    ptypes_t type = pkt_get_type(pkt); // 00000011
    uint8_t tmp = type << 6;
    uint8_t tr = pkt_get_tr(pkt);
    tr =  (tr << 5);
    uint8_t window = pkt_get_window(pkt); // 00011111

    uint8_t to_write = tmp | tr | window;
    memcpy(buf, &to_write, 1); // On copie window, tr et type

    uint32_t timestamp = pkt_get_timestamp(pkt);
    uint8_t seqnum = pkt_get_seqnum(pkt);
    if (type == PTYPE_DATA)
    {
        int lengthH = pkt_get_length(pkt);
        uint16_t lengthN = htons(lengthH);
        memcpy(buf+1, &lengthN, 2);
        memcpy(buf+3, &seqnum, 1);
        memcpy(buf+4, &timestamp, 4);
        uint32_t crc1 = crc32(0L, Z_NULL, 0);
        crc1 = crc32(crc1, (Bytef *) buf, 8);
        crc1 = htonl(crc1);
        memcpy(buf+8, &crc1, 4);
        if (lengthH)
        {
            memcpy(buf+12, pkt_get_payload(pkt), lengthH);
            uint32_t crc2 = crc32(0L, Z_NULL, 0);
            crc2 = crc32(crc2, (Bytef *) buf+12, lengthH);
            crc2 = htonl(crc2);
            memcpy(buf+12+lengthH, &crc2, 4);
            *len = 16 + lengthH;
        }
    }

    else
    {
        memcpy(buf+1, &seqnum, 1);
        memcpy(buf+2, &timestamp, 4);
        uint32_t crc1 = crc32(0L, Z_NULL, 0);
        crc1 = crc32(crc1, (Bytef *) buf, 6);
        crc1 = htonl(crc1);
        memcpy(buf+6, &crc1, 4);
        *len = 10;
    }
    return PKT_OK;
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return (ptypes_t) pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->crc1;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->payload;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->crc2;
}

pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    /* Your code will be inserted here */
    if (type != PTYPE_DATA && type != PTYPE_ACK && type != PTYPE_NACK)
    {
        return E_TYPE;
    }
    pkt->type = type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    /* Your code will be inserted here */
    if (tr != 0 && tr != 1)
    {
        return E_TR;
    }
    pkt->tr = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    /* Your code will be inserted here */
    if (window > MAX_WINDOW_SIZE)
    {
        return E_WINDOW;
    }
    pkt->window = window;
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    /* Your code will be inserted here */
    pkt->seqnum = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    /* Your code will be inserted here */
    if (length > MAX_PAYLOAD_SIZE)
    {
        return E_LENGTH;
    }
    pkt->length = length;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    /* Your code will be inserted here */
    pkt->timestamp = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    /* Your code will be inserted here */
    pkt->crc1 = crc1;
    return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    /* Your code will be inserted here */
    pkt->payload = malloc(length);
    memcpy(pkt->payload, data, length);
    pkt_set_length(pkt, length);
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    /* Your code will be inserted here */
    pkt->crc2 = crc2;
    return PKT_OK;
}

ssize_t predict_header_length(const pkt_t *pkt)
{
    uint16_t length = pkt_get_length(pkt);
    if (pkt_get_type(pkt) != PTYPE_DATA)
    {
        return 0;
    }
    if (length <= MAX_PAYLOAD_SIZE)
    {
        return length;
    }
    return -1;
}


/*
int main(int argc, char const *argv[])
{
    pkt_t* pkt = pkt_new();

    // test pkt_encode
    pkt_set_type(pkt, PTYPE_ACK);
    pkt_set_tr(pkt, 0);
    pkt_set_window(pkt, 27);
    pkt_set_seqnum(pkt, 102);
    pkt_set_timestamp(pkt, 120);
    // pkt_set_payload(pkt, "bonjour,aaudhhhjsklp", 21);

    size_t len = 20;
    char buf[len];
    pkt_status_code code = pkt_encode(pkt, buf, &len);
    FILE *f = fopen("my_file", "w");
    fwrite(buf, 1, len, f);
    fclose(f);
    if (code != PKT_OK){
        printf("c'est pas gud : %d\n\n", code);
    }
    pkt_del(pkt);

    // test pkt_decode
    pkt_t* pkt2 = pkt_new();
    pkt_status_code code2 = pkt_decode(buf, len, pkt2);
    if (code2 != PKT_OK){
        printf("c'est pas gud : %d\n", code2);
    }
    printf("pkt type : %d\n", pkt_get_type(pkt2));
    printf("pkt tr : %d\n", pkt_get_tr(pkt2));
    printf("pkt window : %d\n", pkt_get_window(pkt2));
    printf("pkt seqnum : %d\n", pkt_get_seqnum(pkt2));
    printf("pkt timestamp : %d\n", pkt_get_timestamp(pkt2));
    pkt_del(pkt2);
    return 0;
}*/