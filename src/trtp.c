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
    pkt_t *rcv_packet;
    pkt_set_type(send_packet, PTYPE_DATA);
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
        if(poll_files_descriptors[0].revents & POLLIN){
            //readable et il y qqch
            memset((void *) buffer , 0 , buffer_size);
            n = fread(buffer , 1, buffer_size, fptr);
            if ( feof(fptr)){
                fprintf(stderr , "(read _write_loop) end of file");
                return;
            }
            if(n == 0){
                fprintf(stderr, "nothing read ");
            }
            if(pkt_set_payload(send_packet,buffer,n)!= PKT_OK)
                fprintf(stderr,"erreur de set payload \n");
            char data[16+n];
            int data_size = 0;

            fprintf(stderr,"length : %d\n",pkt_get_length(send_packet));
            if(pkt_encode(send_packet,buffer,(size_t *)&data_size) !=PKT_OK){
                fprintf(stderr,"erreur encode\n");
            }

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

            int sent_status = recv(sfd, &rcv_packet, sizeof (struct pkt_t*), 0);
            if(sent_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            pkt_decode(buffer,buffer_size,rcv_packet);
            pkt_set_window(send_packet,pkt_get_window(send_packet)+1);
            pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
            available_windows = pkt_get_window(rcv_packet);
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
    poll_files_descriptors[1].fd  = sfd;
    poll_files_descriptors[1].events = POLLIN;
    pkt_t *send_packet = pkt_new();
    pkt_t *rcv_packet;
    pkt_set_type(send_packet, PTYPE_ACK);
    pkt_set_tr(send_packet, 0 );
    pkt_set_window(send_packet, 0);
    pkt_set_seqnum(send_packet, 0);
    pkt_set_timestamp(send_packet, 120);
    char buffer[1024];
    int buffer_size = 1024;
    fcntl(sfd, F_SETFL, O_NONBLOCK);
    fcntl(1, F_SETFL, O_NONBLOCK);
    while(1){
        stdin_stdout = poll(poll_files_descriptors, 1 , -1);

        if(poll_files_descriptors[1].revents & POLLIN ){//

            int sent_status = recv(sfd, &rcv_packet, sizeof (struct pkt_t*), 0);
            if(sent_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            pkt_decode(buffer,buffer_size,rcv_packet);
            //cas tronqué
            if(pkt_get_tr(rcv_packet) == 1){
                pkt_set_type(send_packet,PTYPE_NACK);
                pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
                pkt_encode(send_packet,NULL,0);
                send(sfd, send_packet,sizeof(struct pkt_t*), 0 );
            }
            //insere dans la ll et renvoie
            seqnum = pkt_get_seqnum(rcv_packet);
            pkt_set_seqnum(send_packet,seqnum);
        }

    }

}

