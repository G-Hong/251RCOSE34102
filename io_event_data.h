//외부 I/O 이벤트 시나리오 정의만 따로
#ifndef IO_EVENT_DATA_H
#define IO_EVENT_DATA_H

#include "process.h"

typedef struct {
    int trigger_time;
    int pid;
    int burst_time;
} IOEvent;

extern IOEvent io_events[];
extern const int NUM_IO_EVENTS;

#endif
