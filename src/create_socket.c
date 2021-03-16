//
// Created by cam sophic on 04/03/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
///connect and bind a server socket



int create_socket(struct sockaddr_in6 *source_addr,
                  int src_port,
                  struct sockaddr_in6 *dest_addr,
                  int dst_port){

    int sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock == -1){
        fprintf(stderr , "(create socket)erreur socket\n");

        return -1;
    }

    if(source_addr != NULL ){


        source_addr->sin6_family = AF_INET6;
        if(src_port >= 0){
            source_addr->sin6_port = htons(src_port);
        }


        int bind_st = bind(sock, (struct sockaddr *)source_addr, sizeof(*source_addr));

        if(bind_st == -1){
            fprintf(stderr ,"(create socket)error bind");
            perror("hello");

            return -1;
        }

        return sock;
    }


    else if (dest_addr != NULL){

        dest_addr->sin6_family = AF_INET6;
        if(dst_port >= 0 ){
            dest_addr->sin6_port = htons(dst_port);
        }


        int connection_status = connect(sock, (struct sockaddr *) dest_addr, sizeof(*dest_addr));
        if(connection_status == -1){
            //printf("erreur connect");
            fprintf(stderr, "unable to connect (create a socket) \n");

            return -1;
        }

        return sock;
    }

    return -1;
}

