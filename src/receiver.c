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
    int window_available = 7; //WINDOW SIZE => MAX IS 31
    int actual_window = 0;
    poll_files_descriptors[0].fd  = sfd;
    poll_files_descriptors[0].events = POLLIN;
    pkt_t *rcv_packet = pkt_new();
    char buffer[1050];
    int is_empty_buff[256];
    for (int i = 0; i < 256; ++i) {
        is_empty_buff[i] = 0;
    }
    char payload_window[256][512];
    int buffer_size = 1050;
    int first_message = 1;
    int poll_count;

    while(1){
        poll_count = poll(poll_files_descriptors, 1,2000);

        if (poll_count == -1) {
            perror("poll error listening occured, breaking the loop");
            pkt_del(rcv_packet);
            return;
        }

        if(poll_files_descriptors[0].revents & POLLIN ){


            int receive_status = recv(sfd, buffer, buffer_size, 0);
            if(receive_status == -1){
                fprintf(stderr,"nothing received");
                fflush(stdout);
                return;
            }



            if (pkt_decode(buffer,receive_status,rcv_packet)!= PKT_OK){
                perror("error with the encode of decode receiver\n");
                continue;
            }
            int seqnum = pkt_get_seqnum(rcv_packet);
            //mauvais pacquet
            if (pkt_get_type(rcv_packet) != PTYPE_DATA){
                perror("wrong type, ignoring the packet\n");
                continue;
            }
            //erreur d'index
            if ((actual_window + window_available)%256 < actual_window) {
                if (seqnum < actual_window && seqnum > (actual_window + window_available) % 256) {
                    perror("wrong seqnum, ignoring the packet\n");
                    continue;
                }
            }
            else if (seqnum > actual_window + window_available
                     || seqnum < actual_window){
                perror("wrong seqnum, ignoring the packet\n");
                continue;
            }
            fprintf(stderr,"taille du message recu : %d\n",receive_status);


            //CREATE THE FIRST ACK

                //end of file
                if(pkt_get_length(rcv_packet)==0 && pkt_get_tr(rcv_packet) ==0){
                    fprintf(stderr, "\n END OF FILE \n");
                    pkt_t *send_packet = pkt_new();
                    pkt_set_type(send_packet,PTYPE_ACK);
                    pkt_set_tr(send_packet, 0);
                    pkt_set_window(send_packet, window_available);
                    pkt_set_seqnum(send_packet, pkt_get_seqnum(rcv_packet)+1%255);
                    pkt_set_timestamp(send_packet, 100);
                    pkt_set_length(send_packet, 0);
                    size_t len = 10;
                    char to_send[len];
                    pkt_encode(send_packet, to_send, &len);
                    int sent_status = send(sfd, to_send,len, 0 );
                    if(sent_status == -1){
                        perror("ack not sent");
                    }
                    pkt_del(send_packet);
                    pkt_del(rcv_packet);
                    return;
                }

                if (is_empty_buff[seqnum%256] != 0){
                    perror("packet duplicated, ignoring packet");
                    continue;
                }

                //CASE ACK
                if(pkt_get_tr(rcv_packet) != 1){//not truncated

                    is_empty_buff[seqnum%256] = pkt_get_length(rcv_packet);
                    memcpy(payload_window[seqnum%256], pkt_get_payload(rcv_packet), pkt_get_length(rcv_packet));
                    for (int i = actual_window; i < actual_window+32; ++i) {
                        // case we don't have the packet for this indice
                        if (is_empty_buff[i % 256] == 0){
                            fprintf(stderr, "new first slide win : %d \n", i%256);
                            actual_window  = i % 256;
                            break;
                        }
                        else{
                            fprintf(stderr, "========================\n");
                            int write_status = fwrite(pkt_get_payload(rcv_packet),1,pkt_get_length(rcv_packet),stdout);
                            fflush(stdout);
                            fprintf(stderr, "============================\n");

                            if(write_status <= 0){
                                printf("error writing to stdout \n");
                                fflush(stdout);
                            }
                            is_empty_buff[i%256] = 0;
                        }
                    }

                    int window_size = 0;
                    for (int i = actual_window; i < actual_window + 31; ++i) {

                        // we don't have the packet for this indice
                        if (is_empty_buff[i % 256] == 0){
                            window_size += 1;
                        }
                    }
                    fprintf(stderr, "new window size : %d \n", window_size);
                    fprintf(stderr, "sending ack of the received packet\n");
                    pkt_t *send_packet = pkt_new();
                    pkt_set_type(send_packet,PTYPE_ACK);
                    pkt_set_tr(send_packet, 0);
                    pkt_set_window(send_packet, window_size);
                    pkt_set_seqnum(send_packet, pkt_get_seqnum(rcv_packet)+1%255);
                    pkt_set_timestamp(send_packet, 100);
                    pkt_set_length(send_packet, 0);
                    size_t len = 10;
                    char to_send[len];
                    pkt_encode(send_packet, to_send, &len);
                    int sent_status = send(sfd, to_send,len, 0 );
                    if(sent_status == -1){
                        perror("ack not sent");
                    }
                    pkt_del(send_packet);
                    //end of file
                }
                //CASE NACK
                else{
                    pkt_t *pkt = pkt_new();
                    pkt_set_type(pkt, PTYPE_NACK);
                    pkt_set_tr(pkt, 0);
                    pkt_set_window(pkt, window_available);
                    pkt_set_seqnum(pkt, pkt_get_seqnum(rcv_packet));
                    pkt_set_timestamp(pkt, 100);
                    pkt_set_length(pkt, 0);

                    size_t len = 10;
                    char to_send[len];
                    pkt_encode(pkt, to_send, &len);

                    //socklen_t size = sizeof(struct sockaddr);
                    //int written = sendto(sock, to_send, len, 0, peer, size);
                    int written = send(sfd, to_send, len, 0);
                    if (written == -1){
                    }
                    pkt_del(pkt);
                }
            }
    }
    pkt_del(rcv_packet);
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
