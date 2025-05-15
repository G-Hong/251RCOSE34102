//스케쥴링 알고리즘 구현 및 time 연산
#include <stdio.h>
#include "scheduler.h"

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

void run_fcfs(Process processes[], int n) {
    sort_by_arrival_time(processes, n);
    calculate_times(processes, n);
}
void print_gantt_chart(Process processes[], int n) {
    printf("Gantt Chart:\n|");
    for (int i = 0; i < n; i++) {
        printf("  %s  |", processes[i].name);
    }
    printf("\n0");

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;
        current_time += processes[i].burst_time;
        printf("     %d", current_time);
    }
    printf("\n\n");
}

void print_result_table(Process processes[], int n) {
    printf("PID\tName\tArrival\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%s\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].name,
               processes[i].arrival_time,
               processes[i].burst_time,
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

