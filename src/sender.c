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

void send_package(int sfd,char*filename){
    //create file pointer
    FILE* fptr;
    if(filename != NULL){
        fptr = fopen(filename,"r");}
    else if(filename == NULL){fptr = stdin;}
    int fd = fileno(fptr);
    int n;
    struct pollfd poll_files_descriptors[2];
    int stdin_stdout;
    int receiver_window_space= 0;
    int receiver_window_max = 1;
    int oldest_seqnum = 0;
    int acutal_seqnum = 0;
    int sent = 0;
    int received = 0;

    //packet create
    pkt_t *send_packet = pkt_new();
    pkt_t *rcv_packet = pkt_new();
    pkt_set_type(send_packet, PTYPE_DATA); //1 = data
    pkt_set_tr(send_packet, 0 );
    pkt_set_window(send_packet, 1);
    pkt_set_seqnum(send_packet, 0);
    pkt_set_timestamp(send_packet, 120);

    //int seqnum = pkt_get_seqnum(send_packet);
    //
    int buffer_size = 512;
    char buffer[buffer_size];
    char payload[512];
    char ** buffer_window = malloc(sizeof (char*));
    buffer_window[0] = malloc(512*sizeof (char ));
    int *buffer_seqnum = malloc(sizeof(int));
    buffer_seqnum[0] = 0;
    int first_ack = 1;
    //
    //remove blocking operation
    fcntl(fd, F_SETFL, O_NONBLOCK);
    fcntl(sfd, F_SETFL, O_NONBLOCK);






    while(1){
        //create poll descriptors
        poll_files_descriptors[0].fd  = fd;
        poll_files_descriptors[0].events = POLLIN; //Alert me when data is ready to recv() on this socket.

        poll_files_descriptors[1].fd  = sfd;
        poll_files_descriptors[1].events = POLLIN;
        stdin_stdout = poll(poll_files_descriptors, 2 , 50);
        //error on stdin
        if(stdin_stdout == -1){
            perror("poll not working ");
            return;
        }
        //cas de timeout on envoie le oldest
        if(stdin_stdout == 0){
            //On recupere le payload et le seqnum dans les buffer pour renvoyer le packet
            char *buff_Ack = buffer_window[oldest_seqnum];
            int len = 16 + strlen(buff_Ack);
            char data[len];
            pkt_set_seqnum(send_packet,oldest_seqnum);
            pkt_set_payload(send_packet,buff_Ack,strlen(buff_Ack));
            pkt_encode(send_packet,data,(size_t*)&len);
            int sent_status = send(sfd,data,len,0);
            //error on send
            if(sent_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            //looking for lowest seqnum in the buffer
            oldest_seqnum = (oldest_seqnum+1)%255;
            while(buffer_seqnum[oldest_seqnum] == -1){
                oldest_seqnum = (oldest_seqnum+1)%255;
            }
        }

        memset((void *) buffer , 0 , buffer_size);
        //check if something in the stdin and send to socket
        //difference between actual seqnum and the lowest seqnum registered in the buffer in absolute value
        int diff = acutal_seqnum%receiver_window_max-oldest_seqnum%receiver_window_max;
        if(diff <0){
            diff = -diff;
        }
        //We can read file and send to socket + conditions on sliding window + conditions on seqnums
        if(poll_files_descriptors[0].revents & POLLIN && receiver_window_space+1 <= receiver_window_max && diff<=receiver_window_max) { //
            //increasing sliding window and actual seqnum
            receiver_window_space+=1;
            acutal_seqnum = (acutal_seqnum + 1) % 255;
            //receiver_window_space +=1; // for the next iteration
            //readable et il y qqch
            memset((void *) buffer, 0, buffer_size);
            n = fread(buffer, 1, buffer_size, fptr);
            if (n == 0) {
                fprintf(stderr, "nothing read ");
                continue;
            }
            if (pkt_set_payload(send_packet, buffer, n) != PKT_OK){
                fprintf(stderr, "erreur de set payload \n");
            }
            pkt_set_seqnum(send_packet,acutal_seqnum);
            //encoding packet
            int data_initial = 16 + n;
            char data[data_initial];
            int data_size = 16+n;
            if(pkt_encode(send_packet, data ,(size_t *)&data_size) !=PKT_OK){
                fprintf(stderr,"erreur encode\n");
            }
            //check if initiated size is the same as sent size
            if(data_initial!= data_size){
                perror("error with allocating and memory encoding \n");
            }
            //sending packet
            int send_status = send(sfd,data,data_size, 0 );
            if(send_status !=0){
                sent ++;}
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            //put the payload and seqnum in buffer
            buffer_window[pkt_get_seqnum(send_packet)%receiver_window_max] = buffer;
            buffer_seqnum[pkt_get_seqnum(send_packet)%receiver_window_max] = pkt_get_seqnum(send_packet);
            fprintf(stderr,"seqnum de la data : %d \n",pkt_get_seqnum(send_packet));
            memset((void *) buffer , 0 , buffer_size);
        }
        //case we received something in the socket(ACK/NACK)
         if(poll_files_descriptors[1].revents & POLLIN ){// ack nack
            char recv_buff[1024];
            int recv_status = recv(sfd, recv_buff, 1024, 0);
            if(recv_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            //fprintf(stderr,"Ack received \n");
            pkt_decode(recv_buff,recv_status,rcv_packet);
            fprintf(stderr, "RESPONSE OF RECEIVER SEQ : %d\n", pkt_get_seqnum(rcv_packet));
            //ACK
            if(pkt_get_type(rcv_packet) == PTYPE_ACK){
                received++;
                //update window details
                if(first_ack)
                {
                    fprintf(stderr, "ack seqnum : %d", pkt_get_seqnum(rcv_packet));
                    first_ack = 0;
                    receiver_window_max = pkt_get_window(rcv_packet);

                    free(buffer_window);
                    free(buffer_seqnum);
                    buffer_seqnum = malloc(receiver_window_max*sizeof(int));
                    //buffer_seqnum[0] = pkt_get_seqnum(rcv_packet);

                    *buffer_window = (char *) malloc(receiver_window_max * sizeof(char *));
                    for (int i = 0; i < receiver_window_max; i++) {
                        buffer_window[i] = malloc(MAX_PAYLOAD_SIZE * sizeof(char));
                    }

                }

                fprintf(stderr, "max : %d , space : %d\n", receiver_window_max , receiver_window_space);
                receiver_window_space -= 1 ;
                fprintf(stderr, "max : %d , space : %d\n", receiver_window_max , receiver_window_space);
                //setting new seqnum for send_packet
                //pkt_set_window(send_packet,pkt_get_window(rcv_packet));
                //condition for the window !!!
                //moving lowest seqnum to the next lower seqnum not received
                int ack_seqnum = (pkt_get_seqnum(rcv_packet));
                buffer_seqnum[pkt_get_seqnum(rcv_packet)] = -1;
                if((oldest_seqnum) == (ack_seqnum) ){
                    oldest_seqnum = (ack_seqnum+1)%255;
                    while(buffer_seqnum[oldest_seqnum] == -1){
                        oldest_seqnum = (oldest_seqnum+1)%255;
                    }
                }
            }
            //NACK
            else if(pkt_get_type(rcv_packet) == PTYPE_NACK){
                //sending the packet non-acknowledged
                char *buff_Ack = buffer_window[pkt_get_seqnum(rcv_packet)];
                int len = 16 + strlen(buff_Ack);
                char data[len];
                pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
                pkt_set_payload(send_packet,buff_Ack,strlen(buff_Ack));
                pkt_encode(send_packet,data,(size_t*)&len);
                int sent_status = send(sfd,data,len,0);
                if(sent_status == -1 ){
                    fprintf(stderr, "nothing sent");
                }
                memset((void *) buffer , 0 , buffer_size);
            }
            else{
                perror("Sender received data type \n");
                return;
            }
         }
        if(feof(fptr)){
            //wait for all acknowledgement
            if(sent != received)
                continue;
            pkt_set_length(send_packet,0);
            char data[16];
            int data_size = 16;
            pkt_encode(send_packet,data ,(size_t *)&data_size);
            int send_status = send(sfd,data,data_size, 0 );
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            pkt_del(send_packet);
            pkt_del(rcv_packet);
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
    send_package(sfd,filename);

    close(sfd);
    return EXIT_SUCCESS;
}
