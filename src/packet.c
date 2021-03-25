//
// Created by cam sophic on 09/03/2021.
//


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include "packet_interface.h"
#include <arpa/inet.h>
#include <zlib.h>


#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */

#include <sys/types.h>
#include <unistd.h>

#include "packet.h"

struct __attribute__((__packed__)) pkt {

    uint8_t TYPE :2;
    uint8_t TR :1;
    uint8_t WINDOW :5;
    uint16_t LENGTH :16;
    uint8_t SEQNUM :8;
    uint32_t TIMESTAMP :32;
    uint32_t CRC1 :32;
    char* PAYLOAD;
    uint32_t CRC2 :32;

};

/* Alloue et initialise une struct pkt
 * @return: NULL en cas d'erreur */
pkt_t* pkt_new()
{
    return (pkt_t*) malloc(sizeof(pkt_t));

}
/* Libere le pointeur vers la struct pkt, ainsi que toutes les
 * ressources associees
 */
void pkt_del(pkt_t *pkt)
{
    if(pkt != NULL){
        if(pkt->PAYLOAD != NULL){
            free(pkt->PAYLOAD);
        }
        free(pkt);
    }

}


/*
 * Decode des donnees recues et cree une nouvelle structure pkt.
 * Le paquet recu est en network byte-order.
 * La fonction verifie que:
 * - Le CRC32 du header recu est le meme que celui decode a la fin
 *   du header (en considerant le champ TR a 0)
 * - S'il est present, le CRC32 du payload recu est le meme que celui
 *   decode a la fin du payload
 * - Le type du paquet est valide
 * - La longueur du paquet et le champ TR sont valides et coherents
 *   avec le nombre d'octets recus.
 *
 * @data: L'ensemble d'octets constituant le paquet recu
 * @len: Le nombre de bytes recus
 * @pkt: Une struct pkt valide
 * @post: pkt est la representation du paquet recu
 *
 * @return: Un code indiquant si l'operation a reussi ou representant
 *         l'erreur rencontree.
 */
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    ///LOOK FOR ERRORS IN THE FRAME
    if(pkt == NULL){
        perror("no header (pkt decode)");
        return E_UNCONSISTENT;
    }


    //E_TYPE
    uint8_t type = (data[0]>> 6) & 0x03;
    //fprintf(stderr,"%d\n",type);
    //size for ack and nack
    if((type == PTYPE_ACK || type == PTYPE_NACK) && len < 10){
        perror("no header(decode) ");
        return E_NOHEADER;
    }

    // size for data
    if(type == PTYPE_DATA && len < 12){
        perror("no header (decode)");
        return E_NOHEADER;
    }


    //now check if type is compatable with tr
    //only data can be truncated
    uint8_t tr  = (data[0] & 0x20) >> 5 ;
    if(type != PTYPE_DATA  &&  tr ){
        fprintf(stderr, "error here in line 107");
        perror("error with type and truncation (decode) ");
        return E_UNCONSISTENT;
    }


    //SETTIGN IN THE HEADER AND CHECKING FOR ERRORS
    /*
     * TYPE
     * TR
     * WINDOW
     * LENGTH
     * SEQNUM
     * TIMESTAP
     * CRC1
     * PAYLOAD
     * CRC2
     */
    if(pkt_set_type(pkt, type) != PKT_OK){
        perror("error with setting type (pkt_decode)");
        perror("here");

        return E_TYPE;
    }
    if(pkt_set_tr(pkt, tr) != PKT_OK){
        perror("error with tr (decode )");
        return E_TR;
    }
    uint8_t window =  data[0] & 0x1F;

    if(pkt_set_window(pkt, window) != PKT_OK){
        perror("error with the window (decode)");
        return E_WINDOW;
    }

    if(type == PTYPE_DATA){
        uint16_t  length;
        memcpy(&length, data+1, 2);
        length = ntohs(length);
        if((size_t) length + 16 != len){
            perror("not the right length decode");
            return E_LENGTH;
        }
        if(pkt_set_length(pkt, length) == E_LENGTH){
            perror("error settign length (decodee)\n");
            return E_LENGTH;
        }

        uint8_t seqnum = data[3];
        if(pkt_set_seqnum(pkt, seqnum) != PKT_OK){
            perror("error with seqnum");
            return E_SEQNUM;
        }

        uint32_t timestamp;
        memcpy(&timestamp, data + 4, 4);
        pkt_set_timestamp(pkt,timestamp); // doesn't have any error


        //crc1 to check the header
        uint32_t crc1;
        memcpy(&crc1, data + 8, 4 );
        crc1  = ntohl(crc1);
        //crc protocol check
        uint32_t crc_check = crc32(0L, Z_NULL, 0);
        crc_check = crc32(crc_check, (Bytef *) data, 8);

        //check if crc si correct
        if(crc_check != crc1){
            perror("error with the crc1");
            return E_CRC;
        }

        pkt_set_crc1(pkt,crc1);

        //time to decode all the data
        if(length > 0 ){
            //chck the crc2 before readign all the data


            uint32_t crc2;
            memcpy(&crc2 , 12+data+length, 4);
            crc2 = ntohl(crc2);

            crc_check =  crc32(0L, Z_NULL, 0);
            crc_check = crc32(crc_check, (Bytef *) data +12 , length);

            if(crc_check != crc2){
                perror("crc2 error \n");
                return E_CRC;
            }

            pkt_set_crc2(pkt, crc2);
            pkt_set_payload(pkt , data +12, length);


        }

    }
        //ack n
    else if((type == PTYPE_NACK) || (type == PTYPE_ACK)){
        //they have no length part
        uint8_t seqnum;
        seqnum = data[1];
        if(pkt_set_seqnum(pkt,seqnum) == E_SEQNUM){
            return E_SEQNUM;
        }

        uint32_t timestamp;
        memcpy(&timestamp , data+2 , 4);
        pkt_set_timestamp(pkt, timestamp);

        //check_crc

        uint32_t crc1;
        memcpy(&crc1, data + 6 , 4);
        crc1 = ntohl(crc1);

        uint32_t crc_check = crc32(0L, Z_NULL, 0);
        crc_check = crc32(crc_check, (Bytef *) data , 6);

        if(crc_check != crc1){
            perror("errror with the crcc1 nack ack\n");
            return E_CRC;
        }
        pkt_set_crc1(pkt,crc1);

    }
    else{
        perror("error with the type ");
        return E_UNCONSISTENT;
    }

    return PKT_OK;
}

/*
 * Encode une struct pkt dans un buffer, prÃƒÂªt a ÃƒÂªtre envoye sur le reseau
 * (c-a-d en network byte-order), incluant le CRC32 du header et
 * eventuellement le CRC32 du payload si celui-ci est non nul.
 *
 * @pkt: La structure a encoder
 * @buf: Le buffer dans lequel la structure sera encodee
 * @len: La taille disponible dans le buffer
 * @len-POST: Le nombre de d'octets ecrit dans le buffer
 * @return: Un code indiquant si l'operation a reussi ou E_NOMEM si
 *         le buffer est trop petit.
 */
pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    //verify error
    // nack ack size
    size_t size_ack = 10;
    size_t size_data = 16;

    if((pkt_get_type(pkt) == PTYPE_ACK || pkt_get_type(pkt) == PTYPE_NACK) && (size_t) *len < size_ack ){
        perror("not enough memory for ack and nack \n");
        return E_NOMEM;
    }

    if((pkt_get_type(pkt) == PTYPE_DATA) && (size_t) *len < size_data){
        perror("not enough memory for data\n");
        return E_NOMEM; 
    }

    ptypes_t type  = pkt_get_type(pkt);


    uint8_t new_type  = type <<6 ; // take the first six bytes
    //fprintf(stderr , " (encode) type : %d  \n", new_type );


    uint8_t tr = pkt_get_tr(pkt);
    tr = tr << 5 ;

    uint8_t window = pkt_get_window(pkt);
    uint8_t type_tr_window_byte = new_type | tr | window;

    memcpy(buf, &type_tr_window_byte, 1 );






    if(type == PTYPE_DATA) {

        int length = pkt_get_length(pkt);
        uint16_t length_to_buffer = htons(pkt_get_length(pkt));// don't forget to transform to network byte order
        memcpy(buf + 1, &length_to_buffer, 2);


        uint8_t seqnum = pkt_get_seqnum(pkt);
        memcpy(buf + 3, &seqnum, 1);

        uint32_t timestamp = pkt_get_timestamp(pkt);
        memcpy(buf + 4, &timestamp, 4);


        //now for the crc1
        uint32_t crc1 = crc32(0L, Z_NULL, 0);
        crc1 = crc32(crc1, (Bytef *) buf, 8);
        crc1 = htonl(crc1);
        memcpy(buf + 8, &crc1, 4);

        if (length > 0) {
//            char *payload = pkt_get_payload(pkt);
            memcpy(buf + 12, pkt_get_payload(pkt), length);

            //make crc2
            uint32_t crc2 = crc32(0L, Z_NULL, 0);
            crc2 = crc32(crc2, (Bytef *) buf + 12, length);
            crc2 = htonl(crc2);

            memcpy(buf + 12 + length, &crc2, 4);

            *len = 16 + length;
        }
    }

    else if(type == PTYPE_ACK || type == PTYPE_NACK ){//ACK NACK
        uint8_t seqnum = pkt_get_seqnum(pkt);
        memcpy(buf+1 , &seqnum, 1);

        uint32_t timestamp = pkt_get_timestamp(pkt);
        memcpy(buf+2 , &timestamp, 4);

        //make it
        uint32_t crc1 = crc32(0L, Z_NULL, 0);
        crc1 = crc32(crc1, (Bytef*) buf, 6);
        crc1 = htonl(crc1);
        memcpy(buf+6, &crc1, 4);

        *len = 10;
    }
    else{
        return E_NOMEM;
    }
    return PKT_OK;
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return (ptypes_t) pkt->TYPE;
    /* Your code will be inserted here */
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{

    /* Your code will be inserted here */
    return pkt->TR;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->WINDOW;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->SEQNUM;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->LENGTH;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->TIMESTAMP;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->CRC1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->CRC2;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    /* Your code will be inserted here */
    return pkt->PAYLOAD;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    /* Your code will be inserted here */
    if( type != PTYPE_ACK && type != PTYPE_NACK && type != PTYPE_DATA){
        perror("wrong type pkt _set_type ");
        return E_TYPE;
    }
    pkt->TYPE = type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    /* Your code will be inserted here */
    if(tr != 1 && tr != 0){
        return E_TR;
    }
    pkt->TR = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    /* Your code will be inserted here */
    if(window > MAX_WINDOW_SIZE){
        return E_WINDOW;
    }
    pkt->WINDOW = window;
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    /* Your code will be inserted here */

//    if(seqnum > MAX_SEQNUM){
//        return E_SEQNUM;
//    }
//    if(seqnum > 255){
//        return E_SEQNUM;
//    }
    pkt->SEQNUM = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    /* Your code will be inserted here */
    if(length > MAX_PAYLOAD_SIZE){
        return E_LENGTH;
    }
    pkt->LENGTH = length;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    /* Your code will be inserted here */
    pkt->TIMESTAMP = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    /* Your code will be inserted here */
    pkt->CRC1 = crc1;
    return PKT_OK;

}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    /* Your code will be inserted here */
//    if(pkt_get_length(pkt) > 0){
//
//    }
    pkt->CRC2 = crc2;
    return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    /* Your code will be inserted here */
    pkt->PAYLOAD = malloc(length);
    memcpy(pkt->PAYLOAD, data , length);
    if(pkt_set_length(pkt,length) == E_LENGTH){
        return E_LENGTH;
    }

    return PKT_OK;
}

ssize_t predict_header_length(const pkt_t *pkt)
{
    /* Your code will be inserted here */
    uint16_t length = pkt_get_length(pkt);
    uint8_t type = pkt_get_type(pkt);
    if(type != PTYPE_DATA){
        return 0;
    }
    else if(length <= MAX_PAYLOAD_SIZE){
        return length;
    }
    return -1;
}