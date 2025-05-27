//외부 I/O 이벤트 시나리오 정의만 따로
#ifndef IO_EVENT_DATA_H
#define IO_EVENT_DATA_H

#include "process.h"

typedef struct {
    int pid;
    int trigger_time;
    int duration;
    int handled;
} IOEvent;

extern IOEvent scenario_io_events1[];
extern const int NUM_IO_EVENTS1;

#endif
