#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H

#include "process.h"
#include "queue.h"

void sort_by_arrival_time(Process processes[], int n);
void check_new_arrivals(Process processes[], int n, int current_time, int arrived[], Queue* q);

void execute_preemptive_step(
    Process processes[],
    int idx,
    int exec_time,
    int *remaining_burst,
    int *current_time,
    Queue *ready_q,
    int *completed
);

void calculate_times(Process processes[], int n);

void print_title(const char* algorithm_name);
void log_gantt_entry(int pid, int start_time, int end_time);
void reset_gantt_log();
void print_gantt_chart(Process processes[], int n);
void print_result_table(Process processes[], int n);

#endif
