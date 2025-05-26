//스케쥴러 관련 함수 선언
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

void run_fcfs(Process processes[], int n);
void run_sjf_preemptive(Process processes[], int n);
void run_priority(Process processes[], int n);
void run_priority_preemptive(Process processes[], int n);
void run_round_robin(Process processes[], int n);
void run_fcfs_with_io(Process processes[], int n);

#endif