//스케쥴링 알고리즘 구현 및 time 연산
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "queue.h"
#include "io_event_data.h" 
#include "scheduler_utils.h"
#include "io_handler.h"
#include "io_log.h"

void run_fcfs_with_io(Process processes[], int n) {
    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int current_time = 0;
    int completed = 0;
    int running_flag = 0;
    Process running;

    sort_by_arrival_time(processes, n);

    for (int i = 0; i < n; i++) {
        enqueue(&ready_q, processes[i]);
    }

    printf("=== FCFS Scheduling with I/O Simulation ===\n\n");

    while (completed < n) {
        // I/O 복귀 체크
        int q_len = queue_size(&io_q);  // 또는 io_q.size (단, enqueue/dequeue에서 관리한다면)
        for (int i = 0; i < q_len; i++) {
            Process io_proc = dequeue(&io_q);
            if (io_proc.io_complete_time <= current_time) {
                log_io_event(current_time, io_proc.pid, "IO Done", -1, -1);
                io_proc.io_complete_time = 0;
                enqueue(&ready_q, io_proc);
            } else {
                enqueue(&io_q, io_proc); // 아직 I/O 진행 중
            }
        }

        if (!running_flag && !is_empty(&ready_q)) {
            running = dequeue(&ready_q);
            running_flag = 1;
            printf("DEBUG: [DISPATCH] pid=%d assigned to CPU\n", running.pid);
            if (running.executed_time == 0)
                running.start_time = current_time;
        }

        // CPU 할당
        if (running_flag) {
            log_gantt_entry(running.pid, current_time, current_time + 1);
            running.executed_time++;
        
            // I/O 발생 조건 확인
            int io_triggered = 0;
            for (int i = 0; i < NUM_IO_EVENTS; i++) {
                if (io_events[i].pid == running.pid && running.executed_time == io_events[i].trigger_time && running.io_complete_time == 0) {
                    running.io_complete_time = current_time + io_events[i].duration;
                    printf("DEBUG: [I/O START] pid=%d at time=%d, duration=%d\n",
                        running.pid, current_time, io_events[i].duration);
                    log_io_event(current_time, running.pid, "IO Start", io_events[i].trigger_time, io_events[i].duration);
                    enqueue(&io_q, running);
                    running_flag = 0;
                    io_triggered = 1;
                    break;
                }
            }
        
            // 종료 조건은 I/O 발생 안 했을 때만 체크
            if (!io_triggered && running.executed_time >= running.burst_time) {
                running.end_time = current_time + 1;
                running.turnaround_time = running.end_time - running.arrival_time;
                completed++;
                running_flag = 0;
                printf("DEBUG: [COMPLETE] pid=%d at time=%d\n", running.pid, current_time);
            }
        }
        

        current_time++;
        printf("DEBUG: time=%d, completed=%d, ready=%d, running_flag=%d\n",
            current_time, completed, !is_empty(&ready_q), running_flag);
     
    }

    calculate_times(processes, n);
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

