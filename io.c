//
#include <stdio.h>
#include "io.h"
#include "io_event.h"

static int io_event_index = 0;

void handle_io_request(Process **running_process, int current_time, Queue *io_queue) {
    if (!(*running_process)) return;

    while (io_event_index < NUM_IO_EVENTS &&
           io_events[io_event_index].trigger_time == current_time) {

        IOEvent event = io_events[io_event_index];

        if (event.pid == (*running_process)->pid) {
            (*running_process)->io_remaining_time = event.burst_time;
            (*running_process)->is_in_io = 1;
            enqueue(io_queue, **running_process);
            *running_process = NULL;
        }

        io_event_index++;
    }
}

void update_io_queue(Queue *io_queue, Queue *ready_queue) {
    int io_q_size = io_queue->rear - io_queue->front;
    for (int i = 0; i < io_q_size; i++) {
        Process p = dequeue(io_queue);
        p.io_remaining_time--;

        if (p.io_remaining_time <= 0) {
            p.is_in_io = 0;
            enqueue(ready_queue, p);
        } else {
            enqueue(io_queue, p);
        }
    }
}

int is_io_done(Process *p) {
    return (p->io_remaining_time <= 0);
}

void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q) {
    handle_io_request(running, current_time, io_q);
    update_io_queue(io_q, ready_q);
}

