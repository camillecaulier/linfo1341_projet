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

#include "real_address.h"


//address => name of website
//rval => structure stock ip in rval


const char * real_address(const char *address, struct sockaddr_in6 *rval)
{
    memset(rval, 0, sizeof(*rval));
    inet_pton(AF_INET6, address, &(rval->sin6_addr));
    return NULL;
}

