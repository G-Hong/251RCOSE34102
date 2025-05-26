// 이벤트 정의
#ifndef IO_EVENT_H
#define IO_EVENT_H
#include "process.h"   // Process 구조체 정의 포함
#include "queue.h"     // Queue 구조체 정의 포함

typedef struct {
    int trigger_time;   // 언제 I/O가 발생하는지 (시뮬레이터 시점 기준)
    int pid;            // 어떤 프로세스가 I/O 요청하는지
    int burst_time;     // I/O에 걸리는 시간
} IOEvent;

void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q);

extern IOEvent io_events[];
extern const int NUM_IO_EVENTS;

#endif
