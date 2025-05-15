//스케쥴링 알고리즘 구현 및 time 연산
#include <stdio.h>
#include "scheduler.h"
#include "queue.h"
#include "scheduler_utils.h"

void run_fcfs(Process processes[], int n) {
    sort_by_arrival_time(processes, n);
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;

        processes[i].waiting_time = current_time - processes[i].arrival_time;

        // 직접 로그 기록
        log_gantt_entry(processes[i].pid, current_time, current_time + processes[i].burst_time);

        current_time += processes[i].burst_time;
        processes[i].turnaround_time = current_time - processes[i].arrival_time;
    }
}

void run_round_robin(Process processes[], int n) {
    sort_by_arrival_time(processes, n);

    Queue ready_q;
    init_queue(&ready_q);

    int remaining_burst[n];
    for (int i = 0; i < n; i++)
        remaining_burst[i] = processes[i].burst_time;

    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    const int TIME_QUANTUM = 2;

    // 초기 도착 프로세스 삽입
    while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
        enqueue(&ready_q, processes[next_arrival_idx++]);

    while (completed < n) {
        if (is_empty(&ready_q)) {
            // 대기 중인 프로세스 없으면 시간 넘김
            if (next_arrival_idx < n)
                current_time = processes[next_arrival_idx].arrival_time;
            while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
                enqueue(&ready_q, processes[next_arrival_idx++]);
            continue;
        }

        Process p = dequeue(&ready_q);
        int idx = p.pid - 1;
        int exec_time = (remaining_burst[idx] > TIME_QUANTUM) ? TIME_QUANTUM : remaining_burst[idx];

        execute_preemptive_step(processes, idx, exec_time, remaining_burst, &current_time, &ready_q, &completed);

        // 새로 도착한 프로세스들 추가
        while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
            enqueue(&ready_q, processes[next_arrival_idx++]);
    }

    free_queue(&ready_q);
}

