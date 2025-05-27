//I/O 요청 처리 및 큐 상태 업데이트 전담
#include <stdio.h>
#include "io_handler.h"
#include "io_event_data.h"
#include "io_log.h"
#include "queue.h"

extern IOEvent io_events[];

void handle_io_request(Process **running, int current_time, Queue *io_q) {
    if (*running == NULL) return;

    for (int i = 0; i < NUM_IO_EVENTS; i++) {
        IOEvent e = io_events[i];

        if (e.pid == (*running)->pid && (*running)->executed_time == e.trigger_time) {
            (*running)->io_complete_time = current_time + e.duration;
            log_io_event(current_time, (*running)->pid, "IO Start", e.trigger_time, e.duration);
            enqueue(io_q, **running);
            *running = NULL;  // CPU 비움
            break;
        }
    }
}

void update_io_queue(int current_time, Queue *io_q, Queue *ready_q) {
    int size = queue_size(io_q);
    for (int i = 0; i < size; i++) {
        Process p = dequeue(io_q);
        if (current_time >= p.io_complete_time) {
            log_io_event(current_time, p.pid, "IO Done", -1, -1);
            enqueue(ready_q, p);
        } else {
            enqueue(io_q, p);
        }
    }
}


void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q) {
    handle_io_request(running, current_time, io_q);
    update_io_queue(current_time, io_q, ready_q);
}
