/***
 * A set of logging macro and functions that can be used.
 */
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
#include "packet.h"


/* Prints `len` bytes starting from `bytes` to stderr */
void dump(const uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len;) {
        fprintf(stderr, "%04x:  ", (int) i);

        for (size_t j = 0; j < 16 && i + j < len; j++) {
            fprintf(stderr, "%02x ", bytes[i + j]);
        }
        fprintf(stderr, "\t");
        for (size_t j = 0; j < 16 && i < len; j++, i++) {
            fprintf(stderr,"%c ", bytes[i]);
        }
        fprintf(stderr,"\n");
    }
}
 const char * real_address(const char *address, struct sockaddr_in6 *rval)
{
    memset(rval, 0, sizeof(*rval));
    inet_pton(AF_INET6, address, &(rval->sin6_addr));
    return NULL;
}

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



void send_package(int sfd,char*filename){
    FILE* fptr;
    fptr = fopen(filename,"r");
    int fd = fileno(fptr);

    int n;

    struct pollfd poll_files_descriptors[2];
    int stdin_stdout;
    pkt_t *send_packet,*rcv_packet;
    pkt_new(send_packet);
    pkt_set_type(send_packet, PTYPE_DATA);
    pkt_set_tr(send_packet, 1 );
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
            while(pkt_get_window(send_packet)==0){}
//            n = read(STDIN_FILENO, (void *) buffer, 1023);
            memset((void *) buffer , 0 , buffer_size);
            n = fread(buffer , 1, buffer_size, fptr);
            if ( feof(stdin)){
                fprintf(stderr , "(read _write_loop) end of file");
                return;
            }
            if(n == 0){
                fprintf(stderr, "nothing read ");
            }
            pkt_encode(send_packet,buffer,n);
            pkt_set_window(send_packet,pkt_get_window(send_packet)-1);
            //send to socket
            int send_status = send(sfd, send_packet,sizeof(struct pkt_t*), 0 );
            if(send_status == -1 ){
                fprintf(stderr, "nothing sent");
            }
            memset((void *) buffer , 0 , buffer_size);
        }
        if(poll_files_descriptors[1].revents & POLLIN ){//

            int sent_status = recv(sfd, rcv_packet, sizeof (struct pkt_t*), 0);
            if(sent_status == -1){
                fprintf(stderr,"nothing sent");
                fflush(stdout);
            }
            pkt_decode(buffer,buffer_size,rcv_packet);
            pkt_set_window(send_packet,pkt_get_window(send_packet)+1);
            pkt_set_seqnum(send_packet,pkt_get_seqnum(rcv_packet));
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
    pkt_t *send_packet,*rcv_packet;
    pkt_new(send_packet);
    pkt_set_type(send_packet, PTYPE_NACK);
    pkt_set_tr(send_packet, 1 );
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

        int sent_status = recv(sfd, rcv_packet, sizeof (struct pkt_t*), 0);
        if(sent_status == -1){
            fprintf(stderr,"nothing sent");
            fflush(stdout);
        }
        pkt_decode(buffer,buffer_size,rcv_packet);
        //cas fin
        if(rcv_packet->tr == 0){
            pkt_set_type(send_packet,PTYPE_ACK);
            pkt_encode(send_packet,NULL,0);
            send(sfd, send_packet,sizeof(struct pkt_t*), 0 );
            return;
        }
        //insere dans la ll et renvoie
        seqnum = rcv_packet->seqnum;
        pkt_set_seqnum(send_packet,seqnum)
    }

    }




}


