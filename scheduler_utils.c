#include <stdio.h>
#include <string.h>
#include "scheduler_utils.h"
#include "io_log.h"
#include "io_event_data.h"

void sort_by_arrival_time(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }
}

void check_new_arrivals(Process processes[], int n, int current_time, int arrived[], Queue* q) {
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && !arrived[i]) {
            enqueue(q, processes[i]);
            arrived[i] = 1;
        }
    }
}

void execute_preemptive_step(
    Process processes[], int idx, int exec_time,
    int *remaining_burst, int *current_time,
    Queue *ready_q, int *completed
) {
    log_gantt_entry(processes[idx].pid, *current_time, *current_time + exec_time);

    if (remaining_burst[idx] == processes[idx].burst_time)
        processes[idx].waiting_time = *current_time - processes[idx].arrival_time;

    *current_time += exec_time;
    remaining_burst[idx] -= exec_time;

    if (remaining_burst[idx] > 0) {
        if (ready_q != NULL)  // 안전하게 체크
            enqueue(ready_q, processes[idx]);
    } else {
        processes[idx].turnaround_time = *current_time - processes[idx].arrival_time;
        (*completed)++;
    }
}


void calculate_times(Process processes[], int n) {
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;

        processes[i].waiting_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
    }
}

void print_title(const char* algorithm_name) {
    printf("=== %s Scheduling Simulation ===\n\n", algorithm_name);
}

GanttEntry gantt_log[MAX_LOG];
int gantt_log_index = 0;

void reset_gantt_log() {
    gantt_log_index = 0;
}

void log_gantt_entry(int pid, int start_time, int end_time) {
    gantt_log[gantt_log_index++] = (GanttEntry){
        .pid = pid,
        .start_time = start_time,
        .end_time = end_time
    };
}

void print_gantt_chart(Process processes[], int n) {
    printf("Gantt Chart:\n|");

    // 1. 이름 출력
    for (int i = 0; i < gantt_log_index;) {
        int j = i + 1;
        while (j < gantt_log_index && gantt_log[j].pid == gantt_log[i].pid)
            j++;

        printf("  %s  |", processes[gantt_log[i].pid - 1].name);
        i = j;
    }
    printf("\n");

    // 2. 시간 출력
    int time = gantt_log[0].start_time;
    printf("%-6d", time);
    for (int i = 0; i < gantt_log_index;) {
        int j = i + 1;
        while (j < gantt_log_index && gantt_log[j].pid == gantt_log[i].pid)
            j++;

        time = gantt_log[j - 1].end_time;
        printf("%-7d", time);
        i = j;
    }
    printf("\n\n");
}

char* get_io_events_str(int pid) {
    static char buffer[128];
    buffer[0] = '\0';

    for (int j = 0; j < io_log_index; j++) {
        if (io_log[j].pid == pid && strcmp(io_log[j].action, "IO Start") == 0) {
            char entry[32];
            sprintf(entry, "(%d,%d)", io_log[j].trigger_time, io_log[j].duration);
            strcat(buffer, entry);
        }
    }

    return (strlen(buffer) == 0) ? "-" : buffer;
}

void print_result_table(Process processes[], int n) {
    printf("PID  Name     Arr  Burst  Prio  IO_Reqs        Wait  Turn\n");

    for (int i = 0; i < n; i++) {
        printf("%-4d %-8s %-5d %-6d %-6d %-14s %-6d %-6d\n",
            processes[i].pid,
            processes[i].name,
            processes[i].arrival_time,
            processes[i].burst_time,
            processes[i].priority,
            get_io_events_str(processes[i].pid),
            processes[i].waiting_time,
            processes[i].turnaround_time);
    }

    float total_waiting = 0, total_turnaround = 0;
    for (int i = 0; i < n; i++) {
        total_waiting += processes[i].waiting_time;
        total_turnaround += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time: %.2f\n", total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround / n);
}
