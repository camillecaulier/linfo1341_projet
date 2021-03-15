/***
 * A set of logging macro and functions that can be used.
 */

struct DATA{
    unsigned int TYPE : 2;
    unsigned int TR : 1;
    unsigned int WINDOW: 5;
    unsigned int LENGTH: 16;  //uint16_t
    unsigned int SEQNEUM: 8;
    unsigned int CRC1 : 32;
    char* PAYLOAD;
    unsigned int CRC2 : 32;


};

struct NA_ACK{
    //unsigned int IsAck : 1;// 1 (true) si c'est ack
    unsigned int TYPE : 2; // 0b10ACK  0b11 NACK
    unsigned int TR : 1;
    unsigned int WINDOW : 5;
//    unsigned int LENGTH : 16;
    //pas de length dans le nack ou ack
    //tot = 8 bit

};
#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */

/* Creates a socket and initialize it
 * @source_addr: if !NULL, the source address that should be bound to this socket
 * @src_port: if >0, the port on which the socket is listening
 * @dest_addr: if !NULL, the destination address to which the socket should send data
 * @dst_port: if >0, the destination port to which the socket should be connected
 * @return: a file descriptor number representing the socket,
 *         or -1 in case of error (explanation will be printed on stderr)
 */
int create_socket(struct sockaddr_in6 *source_addr,
                  int src_port,
                  struct sockaddr_in6 *dest_addr,
                  int dst_port);
void send_package(const int sfd,char* filename);

const char * real_address(const char *address, struct sockaddr_in6 *rval);
int wait_for_client(int sfd);

#ifndef __LOG_H_
#define __LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef _COLOR
/* Want more/other colors? See https://stackoverflow.com/a/3219471 and
 * https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
 */
#define ANSI_COLOR_BRIGHT_RED "\x1b[91m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#else
#define ANSI_COLOR_BRIGHT_RED
#define ANSI_COLOR_CYAN
#define ANSI_COLOR_RESET
#endif

#define _LOG(color, prefix, msg, ...)\
    do {\
        fprintf(stderr, color prefix msg ANSI_COLOR_RESET "\n", ##__VA_ARGS__);\
    } while (0)

#define ERROR(msg, ...) _LOG(ANSI_COLOR_BRIGHT_RED, "[ERROR] ", msg, ##__VA_ARGS__)

#ifdef _DEBUG
#define DEBUG(msg, ...) _LOG(ANSI_COLOR_CYAN, "[DEBUG] ", msg, ##__VA_ARGS__)
#else
#define DEBUG(msg, ...)
#endif

/* Displays an error if `cond` is not true */
/* Maybe it could also stop the program ? */
#define ASSERT(cond) if (!(cond)) { ERROR("Assertion \"%s\" failed at %s:%d", #cond, __FILE__, __LINE__); }

/* Prints `len` bytes starting from `bytes` to stderr */
void dump(const uint8_t *bytes, size_t len);

/* Use this useful macro instead of the bare function*/
#ifdef _DEBUG
#define DEBUG_DUMP(bytes, len) \
    do {                       \
        DEBUG("Dumping %ld bytes from pointer %p at %s:%d", (size_t) len, bytes, __FILE__, __LINE__); \
        dump((const uint8_t *) bytes, (size_t) len); \
    } while (0)
#else
#define DEBUG_DUMP(bytes, len)
#endif


#endif // __LOG_H_
