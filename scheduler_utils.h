#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H

#include "io_event_data.h"
#include "process.h"
#include "queue.h"
#include <stdbool.h> //삭제?

void sort_by_arrival_time(Process processes[], int num_processes);
//void check_new_arrivals(Process processes[], int num_processes, int current_time, int arrived[], Queue* q);
void check_new_arrivals(Process processes[], int num_processes, int current_time, bool arrived[], Queue* q);
void execute_preemptive_step(
    Process processes[],
    int idx,
    int exec_time,
    int *remaining_burst,
    int *current_time,
    Queue *ready_q,
    int *completed
);
void execute_preemptive_step_with_io(
    Process processes[], int idx, int exec_time,
    int *remaining_burst, int *current_time,
    Queue *ready_q, Queue *io_q,
    int *completed, IOEvent *io_events, int n
);

void calculate_times(Process processes[], int num_processes);

void print_title(const char* algorithm_name);
void log_gantt_entry(int pid, int start_time, int end_time);
void reset_gantt_log();
void print_gantt_chart(Process processes[], int num_processes);
void print_result_table(Process processes[], int num_processes);

#endif
