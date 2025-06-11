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

extern IOEvent scenario_io_events[];
extern const int NUM_IO_EVENTS;
extern IOEvent random_scenario_io_events[];
extern int RANDOM_NUM_IO_EVENTS;

void generate_io_events(const Process processes[], int num_processes, IOEvent io_events[], int *io_event_count);
void generate_safe_io_events(const Process processes[], int n, IOEvent io_events[], int *io_event_count);


#endif
