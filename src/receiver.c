#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#include "log.h"
#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "packet.h"
int print_usage(char *prog_name) {
    ERROR("Usage:\n\t%s [-s stats_filename] listen_ip listen_port", prog_name);
    return EXIT_FAILURE;
}




void receive_package(const int sfd){

    struct pollfd poll_files_descriptors[1];
    int stdin_stdout;
    int seqnum;

    int window_available = 7; //WINDOW SIZE => MAX IS 31
    int *buff_seqnum = malloc(window_available*sizeof (int));
    poll_files_descriptors[0].fd  = sfd;
    poll_files_descriptors[0].events = POLLIN;
    pkt_t *send_packet = pkt_new();
    pkt_t *rcv_packet = pkt_new();

    char buffer[1050];
    int buffer_size = 1050;
    fcntl(sfd, F_SETFL, O_NONBLOCK);
    fcntl(1, F_SETFL, O_NONBLOCK);

    int first_message = 1;

    while(1){
        poll_files_descriptors[0].fd  = sfd;
        poll_files_descriptors[0].events = POLLIN;
        stdin_stdout = poll(poll_files_descriptors, 1 , -1);

        memset((void *) buffer, 0, buffer_size);
        if(poll_files_descriptors[0].revents & POLLIN ){



            memset((void *) buffer, 0, buffer_size);
            int receive_status = recv(sfd, buffer, buffer_size, 0);
            if(receive_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }


            if (pkt_decode(buffer,receive_status,rcv_packet)!= PKT_OK){
                perror("error with the encode of decode receiver ");
            }

            if(pkt_get_length(rcv_packet) ==0){
                //end of sender
                pkt_del(rcv_packet);
                pkt_del(send_packet);
                free(buff_seqnum);
                return;
            }



            fprintf(stderr,"taille du message recu : %d\n",receive_status);

            char ack[12];
            int size = 0;

            //CREATE THE FIRST ACK
            if(first_message){//we make the first message for the first ptype data we get
                first_message= 0;
                //if doublon
                if(buff_seqnum[pkt_get_seqnum(rcv_packet)%31] == pkt_get_seqnum(rcv_packet)){
                    continue;
                }
                buff_seqnum[pkt_get_seqnum(rcv_packet)%31] = pkt_get_seqnum(rcv_packet);
                fprintf(stderr , "WE ARE AT FIRST MESSAGE\n");
                pkt_set_type(send_packet,PTYPE_ACK);
                pkt_set_tr(send_packet, 0);
                pkt_set_window(send_packet,window_available);
                pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));

                pkt_encode(send_packet,ack ,(size_t*) &size);
                int sent_status = send(sfd, ack,size, 0 );
                if(sent_status == -1){
                    perror("first ack not sent");
                }
                fprintf(stderr , "sent first ack\n");
                memset((void *) ack, 0, 12);

            }
            else{
                window_available --;
                pkt_set_window(send_packet, window_available);
                if(pkt_get_tr(rcv_packet) != 1){//not truncated
                    fprintf(stderr, "========================\n");
                    int write_status = fwrite(pkt_get_payload(rcv_packet),1,pkt_get_length(rcv_packet),stdout);
                    fflush(stdout);
                    fprintf(stderr, "============================\n");

                    if(write_status <= 0){
                        printf("error writing to stdout \n");
                        fflush(stdout);
                    }

                    window_available ++;
                    pkt_set_type(send_packet,PTYPE_ACK);

                    pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));


                    pkt_encode(send_packet,ack,(size_t *)&size);
                    int sent_status = send(sfd, ack,size, 0 );
                    if(sent_status == -1){
                        perror("first ack not sent");
                    }

                    fprintf(stderr , "sent ack\n");
                    //insere dans la ll et renvoie
                    memset((void *) ack, 0, 12);
                }
                else{
                    pkt_set_type(send_packet,PTYPE_NACK);
                    pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
                    pkt_encode(send_packet,ack,(size_t *)&size);
                    int sent_status = send(sfd, ack,size, 0 );
                    fprintf(stderr , "sent Nack\n");
                    //insere dans la ll et renvoie
                    memset((void *) ack, 0, 12);
                }
            }

        }


    }


}


// gcc receiver.c -o receiver
// ./receiver ipv6 port
// 2a02:2788:1a4:552:eee9:bbda:b6d0:4e08

//messages are sent in network type order
int main(int argc, char **argv) {
    int opt;

    char *stats_filename = NULL;
    char *listen_ip = NULL;
    char *listen_port_err;
    uint16_t listen_port;

    while ((opt = getopt(argc, argv, "s:h")) != -1) {
        switch (opt) {
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

    listen_ip = argv[optind];
    listen_port = (uint16_t) strtol(argv[optind + 1], &listen_port_err, 10);
    if (*listen_port_err != '\0') {
        ERROR("Receiver port parameter is not a number");
        return print_usage(argv[0]);
    }

    ASSERT(1 == 1); // Try to change it to see what happens when it fails
    DEBUG_DUMP("Some bytes", 11); // You can use it with any pointer type

    // This is not an error per-se.
    ERROR("Receiver has following arguments: stats_filename is %s, listen_ip is %s, listen_port is %u",
        stats_filename, listen_ip, listen_port);

    DEBUG("You can only see me if %s", "you built me using `make debug`");
    ERROR("This is not an error, %s", "now let's code!");

    // Now let's code!


    // REGISTER
    struct sockaddr_in6 listener_addr;
    const char *err = real_address(listen_ip,&listener_addr);
    if (err){
        fprintf(stderr,"Could not resolve hostname %s: %s\n",listen_ip,err);
        return EXIT_FAILURE;
    }
    int sfd;
    sfd = create_socket(&listener_addr,listen_port,NULL,-1);
    if(sfd<0){
        fprintf(stderr,"Could not connect the socket after the first message.\n");
        return EXIT_FAILURE;
    }
    int wait_status = wait_for_client(sfd);
    if(wait_status<0){
        fprintf(stderr,"Wait for client failed\n");
        return EXIT_FAILURE;
    }

    receive_package(sfd);
    close(sfd);


    return EXIT_SUCCESS;
}
