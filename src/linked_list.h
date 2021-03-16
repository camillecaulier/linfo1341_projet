//
// Created by cam sophic on 16/03/2021.
//
#include "packet.h"



#ifndef HEAD_LINKED_LIST_H
#define HEAD_LINKED_LIST_H



//structure for linked list
struct Node {
    int seqnum; //consider this as the index
    const char *pkt_data;
    struct Node* next;
};

void put_linked_pkt(int window_index , const char *pkt_data);

void
#endif //HEAD_LINKED_LIST_H
