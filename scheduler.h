//스케쥴러 관련 함수 선언
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include "io_event_data.h"

// void run_fcfs(Process processes[], int num_processes);
// void run_sjf_preemptive(Process processes[], int num_processes);
// void run_priority(Process processes[], int num_processes);
// void run_priority_preemptive(Process processes[], int num_processes);
// void run_round_robin(Process processes[], int num_processes);
void run_fcfs_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events);
void run_round_robin_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events);


#endif