//
// Created by charl on 16-03-21.
//

#ifndef HEAD_TRTP_H
#define HEAD_TRTP_H



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
void receive_package(const int sfd);

const char * real_address(const char *address, struct sockaddr_in6 *rval);
int wait_for_client(int sfd);
#endif //HEAD_TRTP_H