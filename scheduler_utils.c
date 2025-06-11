#include <stdio.h>
#include <string.h>
#include <stdbool.h>  // bool을 쓰기 위해 반드시 필요
#include "scheduler_utils.h"
#include "io_log.h"
#include "io_event_data.h"
#include "io_handler.h"
#define MAX_PROCESS 100


void sort_by_arrival_time(Process processes[], int num_processes) {
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = i + 1; j < num_processes; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }
}

void check_new_arrivals(Process processes[], int num_processes, int current_time, bool arrived[], Queue* q) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].arrival_time <= current_time && !arrived[i]) {
            enqueue(q, processes[i]);
            arrived[i] = true;
        }
    }
}

void execute_preemptive_step_with_io(
    Process processes[], int idx, int exec_time,
    int *remaining_burst, int *current_time,
    Queue *ready_q, Queue *io_q,
    int *completed, IOEvent *io_events, int n
) {
    Process *p = &processes[idx];

    for (int t = 0; t < exec_time; t++) {
        log_gantt_entry(p->pid, *current_time, *current_time + 1);
        p->executed_time++;
        (*current_time)++;
        remaining_burst[idx]--;

        // 실행 중 I/O 요청이 발생하면 처리하고 CPU 반납
        if (check_and_start_io(p, *current_time - 1, io_q, io_events, n)) {
            return;  // I/O 대기 -> CPU 반납
        }

        // 실행 완료 시 처리
        if (remaining_burst[idx] == 0) {
            p->turnaround_time = *current_time - p->arrival_time;
            (*completed)++;
            return;
        }

        // 이 시점에서 우선순위 더 높은 프로세스가 ready 큐에 들어온 경우 preempt
        int size = queue_size(ready_q);
        for (int i = 0; i < size; i++) {
            Process tmp = dequeue(ready_q);
            if (tmp.priority < p->priority) {
                // 더 높은 우선순위 발견 시, 현재 프로세스를 다시 ready 큐에 넣고 preempt
                enqueue(ready_q, tmp);      // 꺼낸 애 다시 넣기
                enqueue(ready_q, *p);       // 현재 실행중이던 애도 넣기
                return;
            } else {
                enqueue(ready_q, tmp);      // 아니면 그냥 다시 넣기
            }
        }
    }

    // 아직 완료되지 않았고 preempt도 안됐으면 다시 큐에 삽입
    if (ready_q != NULL) {
        enqueue(ready_q, *p);
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

void calculate_times(Process processes[], int num_processes) {
    int last_end_times[MAX_PROCESS] = {0};
    int executed_time[MAX_PROCESS] = {0};

    for (int i = 0; i < gantt_log_index; i++) {
        int pid = gantt_log[i].pid;
        int duration = gantt_log[i].end_time - gantt_log[i].start_time;

        if (gantt_log[i].end_time > last_end_times[pid])
            last_end_times[pid] = gantt_log[i].end_time;

        executed_time[pid] += duration;
    }

    for (int i = 0; i < num_processes; i++) {
        int pid = processes[i].pid;
        processes[i].turnaround_time = last_end_times[pid] - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - executed_time[pid];
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

void print_gantt_chart(Process processes[], int num_processes) {
    printf("Gantt Chart:\n|");

    // 1. 이름 출력
    for (int i = 0; i < gantt_log_index;) {
        int j = i + 1;
        while (j < gantt_log_index && gantt_log[j].pid == gantt_log[i].pid)
            j++;
            
        int burst = gantt_log[j - 1].end_time - gantt_log[i].start_time;
        printf(" P%d(%d) |", gantt_log[i].pid, burst);
            
       //printf("  %s  |", processes[gantt_log[i].pid - 1].name);
        i = j;
    }
    printf("\n");

    // 2. 시간 출력
    int time = gantt_log[0].start_time;
    printf("%-8d", time);
    for (int i = 0; i < gantt_log_index;) {
        int j = i + 1;
        while (j < gantt_log_index && gantt_log[j].pid == gantt_log[i].pid)
            j++;

        time = gantt_log[j - 1].end_time;
        printf("%-8d", time);
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

void print_result_table(Process processes[], int num_processes) {
    printf("PID  Name     Arr   Burst  Prio   Wait   Turn   IO_Reqs        \n");

    for (int i = 0; i < num_processes; i++) {
        printf("%-4d %-8s %-5d %-6d %-6d %-6d %-6d %-14s\n",
            processes[i].pid,
            processes[i].name,
            processes[i].arrival_time,
            processes[i].burst_time,
            processes[i].priority,
            processes[i].waiting_time,
            processes[i].turnaround_time,
            get_io_events_str(processes[i].pid));
    }

    float total_waiting = 0, total_turnaround = 0;
    for (int i = 0; i < num_processes; i++) {
        total_waiting += processes[i].waiting_time;
        total_turnaround += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time: %.2f\n", total_waiting / num_processes);
    printf("Average Turnaround Time: %.2f\n", total_turnaround / num_processes);
}
