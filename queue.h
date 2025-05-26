#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct Node {
    Process data;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

void init_queue(Queue* q);
int is_empty(Queue* q);
void enqueue(Queue* q, Process p);
Process dequeue(Queue* q);
Process* dequeue_pointer(Queue* q);
void free_queue(Queue* q); // 메모리 해제용 (추가)

#endif