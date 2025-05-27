//스케쥴링 알고리즘 구현 및 time 연산
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "queue.h"
#include "io_event_data.h" 
#include "scheduler_utils.h"
#include "io_handler.h"
#include "io_log.h"

void run_fcfs_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events) {
    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int current_time = 0;
    int completed = 0;
    int cpu_idle = 0; // Process* running = NULL; 로 했을 때 에러가 떠서 대안으로 이 방식을 택하기로 함..
    Process running;

    // [1] 도착 시간 기준 정렬 후 초기 ready 큐에 등록
    sort_by_arrival_time(processes, num_processes);
    for (int i = 0; i < num_processes; i++) {
        enqueue(&ready_q, processes[i]);
    }

    printf("=== FCFS Scheduling with I/O Simulation ===\n\n");

    while (completed < num_processes) {
        // [2] I/O 완료된 프로세스 ready 큐로 복귀 처리
        process_io_completion(&io_q, &ready_q, current_time);

        // [3] CPU 할당 (idle 상태 && ready 큐에 대기 중인 프로세스 존재 시)
        if (!cpu_idle && !is_empty(&ready_q)) {
            running = dequeue(&ready_q);
            cpu_idle = 1;
            printf("DEBUG: [DISPATCH] pid=%d assigned to CPU\n", running.pid);
            if (running.executed_time == 0)
                running.start_time = current_time;
        }

        // [4] 실행 중인 프로세스 동작
        if (cpu_idle) {
            log_gantt_entry(running.pid, current_time, current_time + 1);
            running.executed_time++;

            // [4-1] I/O 요청 발생 여부 확인 → 요청 시 io_q로 이동
            if (check_and_start_io(&running, current_time, &io_q, io_events, num_io_events)) {
                cpu_idle = 0;  // CPU 비움
            }
            // [4-2] 완료 조건 확인 (I/O가 발생하지 않았고, Burst 완료 시)
            else if (running.executed_time >= running.burst_time) {
                running.end_time = current_time + 1;
                running.turnaround_time = running.end_time - running.arrival_time;
                completed++;
                cpu_idle = 0;
                printf("DEBUG: [COMPLETE] pid=%d at time=%d\n", running.pid, current_time);
            }
        }

        // [5] 시간 증가 및 상태 출력
        current_time++;
        printf("DEBUG: time=%d, completed=%d, ready=%d, running_flag=%d\n",
            current_time, completed, !is_empty(&ready_q), cpu_idle);
    }

    // [6] 최종 대기/반환 시간 계산
    calculate_times(processes, num_processes);
}

void run_round_robin_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events) {
    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    sort_by_arrival_time(processes, num_processes);

    int remaining_burst[num_processes];
    for (int i = 0; i < num_processes; i++)
        remaining_burst[i] = processes[i].burst_time;

    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    const int TIME_QUANTUM = 2;

    // 초기 도착 프로세스 삽입
    while (next_arrival_idx < num_processes && processes[next_arrival_idx].arrival_time <= current_time)
        enqueue(&ready_q, processes[next_arrival_idx++]);

    while (completed < num_processes) {
        // 1. 매 tick마다 I/O 복귀 처리
        process_io_completion(&io_q, &ready_q, current_time);

        // 2. 실행 가능한 프로세스가 없으면 시간 진행
        if (is_empty(&ready_q)) {
            current_time++;
            continue;
        }

        // 3. ready 큐에서 프로세스 하나 꺼내기
        Process p = dequeue(&ready_q);
        int idx = p.pid - 1;  // 원본 배열 접근을 위한 인덱스

        // 4. 타임퀀텀만큼 실행 (단, burst보다 클 수 없음)
        int exec_time = (remaining_burst[idx] > TIME_QUANTUM) ? TIME_QUANTUM : remaining_burst[idx];

        for (int t = 0; t < exec_time; t++) {
            log_gantt_entry(processes[idx].pid, current_time, current_time + 1);

            processes[idx].executed_time++;
            remaining_burst[idx]--;
            current_time++;

            // 5. I/O 발생 시 즉시 I/O 큐로 이동
            if (check_and_start_io(&processes[idx], current_time - 1, &io_q, io_events, num_io_events)) {
                goto RR_CONTINUE;
            }

            // 6. 프로세스 종료 시 처리
            if (remaining_burst[idx] == 0) {
                processes[idx].end_time = current_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                completed++;
                goto RR_CONTINUE;
            }
        }

        // 7. 아직 종료되지 않은 프로세스는 다시 ready 큐로
        enqueue(&ready_q, processes[idx]);

        RR_CONTINUE:
        // 8. 새로운 도착 프로세스 반영
        while (next_arrival_idx < num_processes && processes[next_arrival_idx].arrival_time <= current_time)
            enqueue(&ready_q, processes[next_arrival_idx++]);
    }
}


// void run_fcfs(Process processes[], int n) {

//     sort_by_arrival_time(processes, n);
//     int current_time = 0;

//     for (int i = 0; i < n; i++) {
//         if (current_time < processes[i].arrival_time)
//             current_time = processes[i].arrival_time;

//         processes[i].waiting_time = current_time - processes[i].arrival_time;

//         // 직접 로그 기록
//         log_gantt_entry(processes[i].pid, current_time, current_time + processes[i].burst_time);

//         current_time += processes[i].burst_time;
//         processes[i].turnaround_time = current_time - processes[i].arrival_time;
//     }
// }

// void run_round_robin(Process processes[], int n) {

//     sort_by_arrival_time(processes, n);

//     Queue ready_q;
//     init_queue(&ready_q);

//     int remaining_burst[n];
//     for (int i = 0; i < n; i++)
//         remaining_burst[i] = processes[i].burst_time;

//     int current_time = 0;
//     int completed = 0;
//     int next_arrival_idx = 0;
//     const int TIME_QUANTUM = 2;

//     // 초기 도착 프로세스 삽입
//     while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
//         enqueue(&ready_q, processes[next_arrival_idx++]);

//     while (completed < n) {
//         if (is_empty(&ready_q)) {
//             // 대기 중인 프로세스 없으면 시간 넘김
//             if (next_arrival_idx < n)
//                 current_time = processes[next_arrival_idx].arrival_time;
//             while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
//                 enqueue(&ready_q, processes[next_arrival_idx++]);
//             continue;
//         }

//         Process p = dequeue(&ready_q);
//         int idx = p.pid - 1;
//         int exec_time = (remaining_burst[idx] > TIME_QUANTUM) ? TIME_QUANTUM : remaining_burst[idx];

//         execute_preemptive_step(processes, idx, exec_time, remaining_burst, &current_time, &ready_q, &completed);

//         // 새로 도착한 프로세스들 추가
//         while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
//             enqueue(&ready_q, processes[next_arrival_idx++]);
//     }

//     free_queue(&ready_q);
//     print_gantt_chart(processes, n);
//     print_result_table(processes, n);
// }

// void run_sjf_preemptive(Process processes[], int n) {

//     sort_by_arrival_time(processes, n);

//     int remaining_burst[n];
//     for (int i = 0; i < n; i++)
//         remaining_burst[i] = processes[i].burst_time;

//     int completed = 0;
//     int current_time = 0;

//     while (completed < n) {
//         // 도착한 모든 프로세스 중 실행 가능한 것 중에 남은 시간이 가장 짧은 것을 선택
//         int min_idx = -1;
//         int min_remaining = 1e9;

//         for (int i = 0; i < n; i++) {
//             if (processes[i].arrival_time <= current_time && remaining_burst[i] > 0) {
//                 if (remaining_burst[i] < min_remaining) {
//                     min_remaining = remaining_burst[i];
//                     min_idx = i;
//                 }
//             }
//         }

//         if (min_idx == -1) {
//             // 실행 가능한 프로세스가 없으면 시간만 흐름
//             current_time++;
//             continue;
//         }

//         // 선점형 실행 처리 (한 번에 1 단위씩만 실행)
//         execute_preemptive_step(processes, min_idx, 1, remaining_burst, &current_time, NULL, &completed);
//     }
// }

// void run_priority(Process processes[], int n) {

//     sort_by_arrival_time(processes, n);

//     int current_time = 0;
//     int completed = 0;
//     int is_completed[n];
//     for (int i = 0; i < n; i++) is_completed[i] = 0;

//     while (completed < n) {
//         int idx = -1;
//         int highest_priority = 1e9;

//         for (int i = 0; i < n; i++) {
//             if (processes[i].arrival_time <= current_time && !is_completed[i]) {
//                 if (processes[i].priority < highest_priority) {
//                     highest_priority = processes[i].priority;
//                     idx = i;
//                 }
//             }
//         }

//         if (idx == -1) {
//             current_time++;
//             continue;
//         }

//         processes[idx].waiting_time = current_time - processes[idx].arrival_time;
//         log_gantt_entry(processes[idx].pid, current_time, current_time + processes[idx].burst_time);

//         current_time += processes[idx].burst_time;
//         processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
//         is_completed[idx] = 1;
//         completed++;
//     }
// }

// void run_priority_preemptive(Process processes[], int n) {

//     sort_by_arrival_time(processes, n);

//     int remaining_burst[n];
//     for (int i = 0; i < n; i++)
//         remaining_burst[i] = processes[i].burst_time;

//     int completed = 0;
//     int current_time = 0;

//     while (completed < n) {
//         int idx = -1;
//         int highest_priority = 1e9;

//         // 현재 시간에 도달한 프로세스 중에서 우선순위 가장 높은 것 선택
//         for (int i = 0; i < n; i++) {
//             if (processes[i].arrival_time <= current_time && remaining_burst[i] > 0) {
//                 if (processes[i].priority < highest_priority) {
//                     highest_priority = processes[i].priority;
//                     idx = i;
//                 }
//             }
//         }

//         if (idx == -1) {
//             current_time++;
//             continue;
//         }

//         // 1ms 단위 실행
//         execute_preemptive_step(processes, idx, 1, remaining_burst, &current_time, NULL, &completed);
//     }
// }

