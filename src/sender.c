#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <inttypes.h>
#include <netdb.h>
#include <fcntl.h>

#include "log.h"
#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "packet.h"

#define BUFF_LEN 

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-f filename] [-s stats_filename] receiver_ip receiver_port", prog_name);
    return EXIT_FAILURE;
}

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



    int buffer_size = 512;
    char buffer[buffer_size];

    fcntl(fd, F_SETFL, O_NONBLOCK);
    fcntl(sfd, F_SETFL, O_NONBLOCK);
    //see if you can write to stdout
//    while( !feof(stdin)){
    while(1){

        poll_files_descriptors[0].fd  = fd;
        poll_files_descriptors[0].events = POLLIN; //Alert me when data is ready to recv() on this socket.
        //POLLOUR //Alert me when I can send() data to this socket without blocking.
        poll_files_descriptors[1].fd  = sfd;
        poll_files_descriptors[1].events = POLLIN;

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
            fprintf(stderr,"seqnum de la data : %d \n",pkt_get_seqnum(send_packet));
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

        if(poll_files_descriptors[1].revents & POLLIN ){// ack nack
            char recv_buff[1024];
            int recv_status = recv(sfd, recv_buff, 1024, 0);
            if(recv_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            fprintf(stderr,"Ack received \n");
            pkt_decode(recv_buff,recv_status,rcv_packet);
            pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
            if(pkt_get_type(rcv_packet) == PTYPE_NACK && pkt_get_tr(rcv_packet) == 1){
                //renvoyer le packet avec le seqnum rcv->seqnum
            }
        }
        if(feof(fptr))
            return;


    }
    fclose(fptr);
}


// gcc sender.c -o sender
// ./sender ipv6 port
int main(int argc, char **argv) {
    int opt;

    char *filename = NULL;
    char *stats_filename = NULL;
    char *receiver_ip = NULL;
    char *receiver_port_err;

    uint16_t receiver_port;

    while ((opt = getopt(argc, argv, "f:s:h")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;

            break;
        case 'h':
            return print_usage(argv[0]);
        case 's':
            stats_filename = optarg;
            break;
        default:
            return print_usage(argv[0]);
        }
    }

    if (optind + 2 != argc) {
        ERROR("Unexpected number of positional arguments");
        return print_usage(argv[0]);
    }

    receiver_ip = argv[optind];
    receiver_port = (uint16_t) strtol(argv[optind + 1], &receiver_port_err, 10);
    if (*receiver_port_err != '\0') {
        ERROR("Receiver port parameter is not a number");
        return print_usage(argv[0]);
    }

    ASSERT(1 == 1); // Try to change it to see what happens when it fails
    DEBUG_DUMP("Some bytes", 11); // You can use it with any pointer type

    // This is not an error per-se.
    ERROR("Sender has following arguments: filename is %s, stats_filename is %s, receiver_ip is %s, receiver_port is %u",
        filename, stats_filename, receiver_ip, receiver_port);

    DEBUG("You can only see me if %s", "you built me using `make debug`");
    ERROR("This is not an error, %s", "now let's code!");
    // Now let's code!


    // REGISTER

    struct sockaddr_in6 receiver_addr;
    const char *err = real_address(receiver_ip,&receiver_addr);
    if (err){
        fprintf(stderr,"Could not resolve hostname %s: %s\n",receiver_ip,err);
        return EXIT_FAILURE;
    }
    int sfd;
    sfd = create_socket(NULL,-1,&receiver_addr,receiver_port);
    if(sfd<0){
        fprintf(stderr,"Could not connect the socket after the first message.\n");
        return EXIT_FAILURE;
    }
    //SOCKET CONNECTED
    //
    //
    //SENDING PAYLOADS
    send_package(sfd,filename);



    close(sfd);
    return EXIT_SUCCESS;
}
