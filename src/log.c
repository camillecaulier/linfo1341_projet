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

struct TRTP *make_packet(void *packet)
{
	uint32_t header;
    struct TRTP *pakage = (struct TRTP*)malloc(sizeof(pakage));
    memcpy(&header, packet, 4);
    header = htonl(header);
    
    
    uint8_t trtp[32];
	int i = 31;
	for (i = 31; i >= 0; i--) trtp[31-i] = ((header >> i) & 1);
    //for (int i = 0; i < 32; i++) printf("%d", bits[i]); printf("\n");

    // Type
	pakage->TYPE = trtp[0] * 2 + trtp[1];

    // Truncated ?
	pakage->TR = trtp[2];
    // Window
	for (i = 0; i < 5; i++) pakage->WINDOW = 2 * pakage->WINDOW + trtp[3 + i];

    // Length
	for (i = 0; i < 16; i++) pakage->LENGTH = 2 * pakage->LENGTH + trtp[8 + i];

    memcpy(&pakage->SEQNEUM, packet + 2 + pakage->LENGTH, 1);
   
	
    // CRC1
    pakage->CRC1 = crc32(0, packet, 7 + pakage->LENGTH);
    memcpy(&pakage->CRC1, packet + 7 + pakage->LENGTH, 4);
    pakage->CRC1 = ntohl(pakage->CRC1);

    // CRC2
    memcpy(&pakage->CRC2, packet + 11 + pakage->LENGTH, 4);
    pakage->CRC2 = ntohl(pakage->CRC2);
	pakage->CRC2 = crc32(0, packet + 11 + 1, pakage->LENGTH);

    // Payload
	
    if(pakage->LENGTH > 0) 
		pakage->PAYLOAD = malloc(pakage->LENGTH);
    memcpy(pakage->PAYLOAD, packet + 11 ,pakage->LENGTH);


    return pakage;
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
