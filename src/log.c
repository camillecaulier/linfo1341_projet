/***
 * A set of logging macro and functions that can be used.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
//#include "log.h"
#include "packet.h"


/* Prints `len` bytes starting from `bytes` to stderr */
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

