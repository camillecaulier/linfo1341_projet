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
#include <sys/wait.h>
#include "log.h"
#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "packet.h"
#include <math.h>

#define BUFF_LEN

int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-f filename] [-s stats_filename] receiver_ip receiver_port", prog_name);
    return EXIT_FAILURE;
}
int getOldestSeqnum(int * isEmpty){
    if(isEmpty[256]!= -1){
        for(int i = 256 ; i>0;i--){
            if (isEmpty[i] == -1){
                return i+1;
            }
        }
    }
    else{
        for(int  i = 1 ; i<257;i++){
            if(isEmpty[i] == -1){
                return i-1;
            }
        }
    }
    return 0;
}

void send_package(int sfd,char*filename,char*output){
    //create file pointer
    FILE* fptr;
    FILE * fptr2;
    if(filename != NULL){
        fptr = fopen(filename,"r");}
    else if(filename == NULL){fptr = stdin;}
    if(output != NULL){
        fptr = fopen(output,"w+");}
    else if(output == NULL){fptr = stderr;}
    int fd = fileno(fptr);
    int fd2 = fileno(fptr2);
    //Stats
    int packet_sent=0;
    int Ack_received = 0;
    int Nack_received = 0;
    int Packet_lost = 0;



    int n;
    struct pollfd poll_files_descriptors[2];
    //create poll descriptors
    poll_files_descriptors[0].fd  = fd;
    poll_files_descriptors[0].events = POLLIN; //Alert me when data is ready to recv() on this socket.

    //POLL on the socket
    poll_files_descriptors[1].fd  = sfd;
    poll_files_descriptors[1].events = POLLIN | POLLOUT;
//        poll_files_descriptors[1].events = POLLIN;

    int stdin_stdout;
    int receiver_window_space= 0;
    int receiver_window_max = 1;
    int oldest_seqnum = 0;


    int received = 0;
    int buffer_size = 512;
    char buffer[buffer_size];
    char payload[512];
    char pckt_window[256][16+512];
    int first_ack = 1;
    int Thelast = 0;
    int actual_seqnum = 0;
    int last_seqnum;
    int acked_seqnum = 0;
    int sent = 0;
    int isEmpty[257];
    for(int i = 0 ; i<257 ; i++){
        isEmpty[i] = -1;
    }

    int last_sent =0;

    //packet create

    pkt_t *rcv_packet = pkt_new();
    int ind = 0;
    while(1){


        if(!Thelast && receiver_window_max > receiver_window_space && ind%256 == acked_seqnum ){
            n = fread(buffer, 1, buffer_size, fptr);
            if (n == 0) {
                Thelast = 1;
                continue;
            }
            fprintf(stderr,"\nactual seqnum : %d\n",actual_seqnum);
            int data_initial = 16 + n;
            char data[data_initial];
            pkt_t *sent_packet = pkt_new();
            pkt_set_type(sent_packet, PTYPE_DATA);
            pkt_set_tr(sent_packet, 0);
            pkt_set_window(sent_packet, 0);
            pkt_set_seqnum(sent_packet, actual_seqnum);
            pkt_set_timestamp(sent_packet, pkt_get_timestamp(rcv_packet));
            pkt_set_payload(sent_packet, buffer, n);
            pkt_encode(sent_packet, data, (size_t *)&data_initial);
            memcpy(pckt_window[actual_seqnum],data,16+512);
            int send_status = send(sfd,data,data_initial, 0 );
            receiver_window_space++;
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            sent++;
            packet_sent++;
            isEmpty[actual_seqnum] = 16+n;
            actual_seqnum = (actual_seqnum + 1) % 256;

        }

        //error on stdin


        //check if something in the stdin and send to socket
        //difference between actual seqnum and the lowest seqnum registered in the buffer in absolute value

        //We can read file and send to socket + conditions on sliding window + conditions on seqnums
        //we also check if we have something in the socket and in the file and if we can write it to the socket




        //case we received something in the socket(ACK/NACK)
        int poll_result = poll(poll_files_descriptors, 2, pkt_get_timestamp(rcv_packet));
        if(poll_result == 0){
            Packet_lost ++;
            int old = getOldestSeqnum(isEmpty);
            int sent_status = send(sfd,pckt_window[old],strlen(pckt_window[old]),0);
            if(sent_status == -1){
                perror("file not sent");
            }


        }
        if(poll_files_descriptors[1].revents & POLLIN ){// ack nack
            char recv_buff[1024];
            //fprintf(stderr,"current buff %d: %s\n",acutal_seqnum,buffer_window[acutal_seqnum%receiver_window_max]);
            int recv_status = recv(sfd, recv_buff, 1024, 0);
            if(recv_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            //fprintf(stderr,"Ack received \n");
            pkt_decode(recv_buff,recv_status,rcv_packet);
            fprintf(stderr, "RESPONSE OF RECEIVER\n");
            //ACK
            if(pkt_get_type(rcv_packet) == PTYPE_ACK){
                Ack_received ++;
                if(pkt_get_seqnum(rcv_packet) == 0){
                    isEmpty[256] = -1;
                }
                else{
                    isEmpty[pkt_get_seqnum(rcv_packet)-1] = -1;
                }
                ind ++;
                received++;
                receiver_window_space --;
                acked_seqnum = pkt_get_seqnum(rcv_packet);
                //update window details
                if(first_ack)
                {
                    fprintf(stderr, "ack seqnum : %d\n", pkt_get_seqnum(rcv_packet));
                    first_ack = 0;
                    receiver_window_max = pkt_get_window(rcv_packet);

                    //buffer_seqnum[0] = pkt_get_seqnum(rcv_packet);
                    fprintf(stderr, "receiver_window : %d\n", receiver_window_max);
                }

                fprintf(stderr, "max : %d , space : %d\n", receiver_window_max , receiver_window_space);

                fprintf(stderr, "max : %d , space : %d\n", receiver_window_max , receiver_window_space);
                //setting new seqnum for send_packet
                //pkt_set_window(send_packet,pkt_get_window(rcv_packet));
                //condition for the window !!!
                //moving lowest seqnum to the next lower seqnum not received
                int ack_seqnum = (pkt_get_seqnum(rcv_packet));
                //fprintf(stderr, "buffer position %d , acked : %s\n",pkt_get_seqnum(rcv_packet)%receiver_window_max, (buffer_window[pkt_get_seqnum(rcv_packet)%receiver_window_max]));

                fprintf(stderr,"i'm actually here");
            }
                //NACK
            else if(pkt_get_type(rcv_packet) == PTYPE_NACK){
                Nack_received ++;
                //sending the packet non-acknowledged

                int sent_status = send(sfd,pckt_window[pkt_get_seqnum(rcv_packet)],strlen(pckt_window[pkt_get_seqnum(rcv_packet)]),0);
                if(sent_status == -1 ){
                    fprintf(stderr, "nothing sent");
                }
                //
                //memset((void *) buffer , 0 , buffer_size);
            }
            else{
                perror("Sender received data type \n");
                return;
            }
        }
        if(feof(fptr)){
            //wait for all acknowledgement
            fprintf(stderr, "in the feof\n");
            if(!last_sent){
            int data_initial = 16 ;
            char data[data_initial];
            pkt_t *sent_packet = pkt_new();
            pkt_set_type(sent_packet, PTYPE_DATA);
            pkt_set_tr(sent_packet, 0);
            pkt_set_window(sent_packet, 0);
            pkt_set_seqnum(sent_packet, actual_seqnum);
            pkt_set_timestamp(sent_packet, pkt_get_timestamp(rcv_packet));
            pkt_set_payload(sent_packet, payload, 0);
            pkt_encode(sent_packet,data ,(size_t *)&data_initial);
            int send_status = send(sfd,data,data_initial, 0 );
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            pkt_del(sent_packet);
            last_sent = 1;
            sent ++;
            packet_sent++;
            }
            if(sent != received){
                continue;
            }
            pkt_del(rcv_packet);
            fprintf(fptr2,"\n================================\n");
            fprintf(fptr2,"Stats:Values\n");
            fprintf(fptr2,"Packet sent:%d\n",packet_sent);
            fprintf(fptr2,"Ack received:%d\n",Ack_received);
            fprintf(fptr2,"Packet truncated:%d",Nack_received);
            fprintf(fptr2,"Packet lost:%d\n",Packet_lost);
            fprintf(fptr2,"\n================================\n");
            fclose(fptr2);

            return;
        }
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
    send_package(sfd,filename,stats_filename);

    close(sfd);
    return EXIT_SUCCESS;
}