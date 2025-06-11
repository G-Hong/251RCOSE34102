#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include "queue.h"

void init_queue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

int is_empty(Queue* q) {
    return (q->front == NULL);
}



void enqueue(Queue* q, Process p) {
    //printf("DEBUG: enqueue() - PID=%d\n", p.pid); // 확인용
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = p;
    new_node->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->size++;
}

Process dequeue(Queue* q) {
    if (is_empty(q)) {
        fprintf(stderr, "Queue underflow\n");
        exit(EXIT_FAILURE);
    }

    Node* temp = q->front;
    Process data = temp->data;

    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    return data;
}

void free_queue(Queue* q) {
    while (!is_empty(q)) {
        dequeue(q);
    }
}


int queue_size(Queue* q) {
    int count = 0;
    Node* cur = q->front;
    while (cur != NULL) {
        count++;
        cur = cur->next;
    }
    return count;
}
