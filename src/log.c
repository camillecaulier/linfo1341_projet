/***
 * A set of logging macro and functions that can be used.
 */

#include "log.h"

/* Prints `len` bytes starting from `bytes` to stderr */
void trtp_init(struct TRTP* trtp){
	trtp->TYPE = 0;
	trtp->TR = 0;
	trtp->WINDOW = 0;
	trtp->LENGTH = 0;
	trtp->SEQNEUM = 0;
	trtp->CRC1 = 0;
	trtp->CRC2 = 0;
	free(&trtp->PAYLOAD);
}

/*
 * display the byte
 */
void display_byte(uint8_t byte)
{
    int j = 0;
    for (j = 0; j < 8;  j++) printf("%d", ((byte >> (7-j)) & 1));
    printf(" ");
}

//On check le data packet et sa taille si c'est plus grand que le payload il faut split

ssize_t send(int sockfd,const void *buf,size_t len, int flags){
	size_t taille = 0;
	//init de la data
	struct TRTP *packet = malloc(sizeof(*packet));
	packet->TYPE = DATA;
	memset(packet->PAYLOAD,'\0',sizeof(packet->PAYLOAD));
	
	//init du ACK
	struct TRTP *ack = malloc(sizeof(*ack));
	ack->TYPE = ACK;
	memset(ack->PAYLOAD,'\0',sizeof(ack->PAYLOAD));
	//init du Nack
	struct TRTP *nack = malloc(sizeof(*nack));
	nack->TYPE = NACK;
	memset(nack->PAYLOAD,'\0',sizeof(nack->PAYLOAD));
	return taille;
	}
/**
 * display in byte repsentation the message/data
 */
void byte_display(void *data, int size){
    uint8_t byte;
    for(int i = 0 ; i < size ; i++){
        memcpy(&byte, data + i, 1);
        display_byte(byte);
    }

    printf("\n");

    for(int i = 0 ; i < size; i++){
        memcpy(&byte, data +i  , 1);
        printf("%02X", byte);
    }
    printf("\n");
    
}

void dump(const uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len;) {
        fprintf(stderr, "%04x:  ", (int) i);

        for (size_t j = 0; j < 16 && i + j < len; j++) {
            fprintf(stderr, "%02x ", bytes[i + j]);
        }
        fprintf(stderr, "\t");
        for (size_t j = 0; j < 16 && i < len; j++, i++) {
            fprintf(stderr,"%c ", bytes[i]);
        }
        fprintf(stderr,"\n");
    }
}
