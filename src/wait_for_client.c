//
// Created by cam sophic on 05/03/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int wait_for_client(int sfd){

    struct sockaddr_in6 client_address;
    int len;
    char buffer[1024];

    len = sizeof(client_address);
    int response_status = recvfrom(sfd, buffer, 1024, MSG_PEEK, (struct sockaddr*)&client_address, (socklen_t *) & len);
    if(response_status < 0){
        fprintf(stderr, "fail of resonse (wait for client)");
        fflush(stdout);
        return -1;
    }
    buffer[response_status] = '\0';


    int connect_status =connect(sfd, (struct sockaddr *)&client_address, sizeof(client_address));
    //error here
    if(connect_status == -1){
        fprintf(stderr, "failed to connect to client ( wait for client) \n");
        fflush(stdout);
        return -1;
    }
    //fprintf(stderr , "%s\n", buffer);
    return 0;
}

