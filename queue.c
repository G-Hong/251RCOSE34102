#include <stdio.h>
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
    if (q == NULL) {
        fprintf(stderr, "FATAL: enqueue called with NULL queue pointer\n");
        exit(1);
    }

    printf("DEBUG: enqueue() - PID=%d\n", p.pid);

    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "FATAL: malloc failed in enqueue\n");
        exit(1);
    }

    // 디버깅용 필드 출력
    printf("       name=%s, arrival=%d, burst=%d, priority=%d\n",
        p.name, p.arrival_time, p.burst_time, p.priority);

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


// void enqueue(Queue* q, Process p) {
//     printf("DEBUG: enqueue() - PID=%d\n", p.pid); // 확인용
//     Node* new_node = (Node*)malloc(sizeof(Node));
//     new_node->data = p;
//     new_node->next = NULL;

//     if (q->rear == NULL) {
//         q->front = q->rear = new_node;
//     } else {
//         q->rear->next = new_node;
//         q->rear = new_node;
//     }
//     q->size++;
// }

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
