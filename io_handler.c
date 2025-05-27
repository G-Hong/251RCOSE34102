//I/O 요청 처리 및 큐 상태 업데이트 전담
#include <stdio.h>
#include "io_handler.h"
#include "io_event_data.h"
#include "io_log.h"
#include "queue.h"

void handle_io_request(Process **running, int current_time, Queue *io_q, IOEvent *io_events, int n) {
    if (*running == NULL) return;

    for (int i = 0; i < n; i++) {
        IOEvent *e = &io_events[i];  // ✅ 구조체 주소 참조 (포인터)

        if (e->pid == (*running)->pid &&
            (*running)->executed_time == e->trigger_time &&
            !e->handled)
        {
            (*running)->io_complete_time = current_time + e->duration;
            log_io_event(current_time, (*running)->pid, "IO Start", e->trigger_time, e->duration);
            e->handled = 1;  // ✅ 원본을 수정!
            enqueue(io_q, **running);
            *running = NULL;
            return;
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

void process_io_completion(Queue *io_q, Queue *ready_q, int current_time) {
    int len = queue_size(io_q);
    for (int i = 0; i < len; i++) {
        Process p = dequeue(io_q);
        if (p.io_complete_time <= current_time) {
            log_io_event(current_time, p.pid, "IO Done", -1, -1);
            p.io_complete_time = 0;
            enqueue(ready_q, p);
        } else {
            enqueue(io_q, p);
        }
    }
}

int check_and_start_io(Process *running, int current_time, Queue *io_q, IOEvent *io_events, int n) {
    for (int i = 0; i < n; i++) {
        if (io_events[i].pid == running->pid &&
            running->executed_time == io_events[i].trigger_time &&
            !io_events[i].handled)
        {
            running->io_complete_time = current_time + io_events[i].duration;
            printf("DEBUG: [I/O START] pid=%d at time=%d, duration=%d\n",
                   running->pid, current_time, io_events[i].duration);
            log_io_event(current_time, running->pid, "IO Start", io_events[i].trigger_time, io_events[i].duration);
            enqueue(io_q, *running);
            return 1;
        }
    }
    return 0;
}

void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q, IOEvent *io_events, int n) {
    handle_io_request(running, current_time, io_q, io_events, n);
    update_io_queue(current_time, io_q, ready_q);
}
