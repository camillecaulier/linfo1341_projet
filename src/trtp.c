//
// Created by charl on 16-03-21.
//
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
#include <poll.h>
#include "packet.h"
#include "trtp.h"


void send_package(int sfd,char*filename){
    FILE* fptr;
    if(filename != NULL){
    fptr = fopen(filename,"r");}
    else if(filename == NULL){fptr = stdin;}

    int fd = fileno(fptr);

    int n;

    struct pollfd poll_files_descriptors[2];
    int stdin_stdout;
    int available_windows = 1;

    pkt_t *send_packet = pkt_new();
    pkt_t *rcv_packet = pkt_new();
    pkt_set_type(send_packet, PTYPE_DATA); //1 = data
    pkt_set_tr(send_packet, 0 );
    pkt_set_window(send_packet, 31);
    pkt_set_seqnum(send_packet, 0);
    pkt_set_timestamp(send_packet, 120);

    poll_files_descriptors[0].fd  = fd;
    poll_files_descriptors[0].events = POLLIN; //Alert me when data is ready to recv() on this socket.

    //POLLOUR //Alert me when I can send() data to this socket without blocking.

    poll_files_descriptors[1].fd  = sfd;
    poll_files_descriptors[1].events = POLLIN;

    char buffer[1024];
    int buffer_size = 1024;
    fcntl(fd, F_SETFL, O_NONBLOCK);
    fcntl(sfd, F_SETFL, O_NONBLOCK);
    //see if you can write to stdout
//    while( !feof(stdin)){
    while(1){


        stdin_stdout = poll(poll_files_descriptors, 2 , -1);
        n = 0;
        if(stdin_stdout == -1){
            perror("poll not working ");
            return;
        }

        if(stdin_stdout == 0){
            perror("poll timed out\n ");
            return;
        }

        memset((void *) buffer , 0 , buffer_size);


        //check if something in the stdin and send to socket
        if(poll_files_descriptors[0].revents & POLLIN) {
            //readable et il y qqch
            memset((void *) buffer, 0, buffer_size);
            n = fread(buffer, 1, buffer_size, fptr);

            if (n == 0) {
                fprintf(stderr, "nothing read ");
            }
            if (pkt_set_payload(send_packet, buffer, n) != PKT_OK){
                fprintf(stderr, "erreur de set payload \n");
            }

            int data_initial = 16 + n;
            char data[data_initial];
            int data_size = 16+n;

            fprintf(stderr,"length : %d\n",pkt_get_length(send_packet));
            if(pkt_encode(send_packet, data ,(size_t *)&data_size) !=PKT_OK){
                fprintf(stderr,"erreur encode\n");
            }
            //check if initiated size is the same as sent size
            if(data_initial!= data_size){
                perror("error with allocating and memory encoding \n");

            }
            fprintf(stderr, "first byte: %d \n", data[0]);
            fprintf(stderr,"taille de data %d\n",data_size);
            fprintf(stderr,"buff = %s\n",data);
            fprintf(stderr,"type de la data : %d \n",pkt_get_type(send_packet));
            available_windows --;

            //send to socket

            fprintf(stderr,"buffer sent : %s\n",pkt_get_payload(send_packet));

            int send_status = send(sfd,data,data_size, 0 );
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            fprintf(stderr,"status sent : %d\n",send_status);

            memset((void *) buffer , 0 , buffer_size);
        }
        if(poll_files_descriptors[1].revents & POLLIN ){//
            char recv_buff[1024];
            int recv_status = recv(sfd, recv_buff, 1024, 0);
            if(recv_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            fprintf(stderr,"Ack received \n");
            pkt_decode(recv_buff,recv_status,rcv_packet);
            if(pkt_get_type(rcv_packet) == PTYPE_NACK && pkt_get_tr(rcv_packet) == 1){
                //renvoyer le packet avec le seqnum rcv->seqnum
            }
        }


    }
    fclose(fptr);
}


void receive_package(const int sfd){

    struct pollfd poll_files_descriptors[1];
    int stdin_stdout;
    int seqnum;
    poll_files_descriptors[0].fd  = sfd;
    poll_files_descriptors[0].events = POLLIN;
    pkt_t *send_packet = pkt_new();
    pkt_t *rcv_packet = pkt_new();

    char buffer[1050];
    int buffer_size = 1050;
    fcntl(sfd, F_SETFL, O_NONBLOCK);
    fcntl(1, F_SETFL, O_NONBLOCK);


    while(1){
        poll_files_descriptors[0].fd  = sfd;
        poll_files_descriptors[0].events = POLLIN;
        stdin_stdout = poll(poll_files_descriptors, 1 , -1);
        fprintf(stderr,"im here\n");
        memset((void *) buffer, 0, buffer_size);
        if(poll_files_descriptors[0].revents & POLLIN ){
            memset((void *) buffer, 0, buffer_size);
            int receive_status = recv(sfd, buffer, buffer_size, 0);
            if(receive_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            pkt_decode(buffer,receive_status,rcv_packet);
            fprintf(stderr,"taille du message recu : %d\n",receive_status);
            fprintf(stderr,"message recu : %s\n",pkt_get_payload(rcv_packet));
            char ack[12];
            int size = 0;
            pkt_set_type(send_packet,PTYPE_ACK);
            pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet)+1%255);
            pkt_encode(send_packet,ack,(size_t *)&size);
            int sent_status = send(sfd, ack,size, 0 );
            fprintf(stderr , "sent ack  : %d\n", sent_status);
            //insere dans la ll et renvoie
            memset((void *) buffer, 0, buffer_size);

        }


    }


}

