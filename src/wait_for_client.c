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
#include "packet.h"
int wait_for_client(int sfd){

    struct sockaddr_in6 client_address;

    int len;
    char buffer[1050];

    len = sizeof(client_address);
    int response_status = recvfrom(sfd, buffer, 1050, 0, (struct sockaddr*)&client_address, (socklen_t *)& len);
    if(response_status < 0){
        fprintf(stderr, "fail of resonse (wait for client)");
        fflush(stdout);
        return -1;
    }
    fprintf(stderr,"buffer : %s\n",buffer);
    buffer[response_status] = '\0';

    pkt_t *received_packet = pkt_new();
    pkt_t *sent_ack = pkt_new();
    pkt_decode(buffer,response_status,received_packet);

    int connect_status =connect(sfd, (struct sockaddr *)&client_address, sizeof(client_address));
    //error here
    if(connect_status == -1){
        fprintf(stderr, "failed to connect to client ( wait for client) \n");
        fflush(stdout);
        return -1;
    }
//    pkt_set_type(sent_ack,PTYPE_ACK);
//    pkt_set_window(sent_ack,31);
//
//    pkt_set_seqnum(sent_ack,7);
//
//    char ack[12];
//    int size = 0;
//    pkt_encode(sent_ack,ack,(size_t *)&size);
//    int sent_status = send(sfd, ack,size, 0 );
//    if(sent_status == -1){
//        perror("we sent fuck all");
//    }
//    fprintf(stderr,"le message est : %s \n",pkt_get_payload(received_packet));
//    fprintf(stderr , "sent ack  : %d\n", sent_status);

    return 0;
}

