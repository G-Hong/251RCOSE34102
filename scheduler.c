//스케쥴링 알고리즘 구현 및 time 연산
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

#include "process.h"
#include "scheduler.h"
#include "queue.h"
#include "io_event_data.h" 
#include "scheduler_utils.h"
#include "io_handler.h"
#include "io_log.h"

void run_fcfs_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {
    print_title("FCFS with I/O");

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    sort_by_arrival_time(processes, n);

    int current_time = 0, completed = 0;
    bool arrived[n];
    for (int i = 0; i < n; i++) {
        arrived[i] = false;
        processes[i].executed_time = 0;
    }

    Process running;
    int cpu_idle = 0;

    while (completed < n) {
        check_new_arrivals(processes, n, current_time, arrived, &ready_q);
        process_io_completion(&io_q, &ready_q, current_time);

        if (!cpu_idle && !is_empty(&ready_q)) {
            running = dequeue(&ready_q);
            cpu_idle = 1;
            if (running.executed_time == 0)
                running.start_time = current_time;
        }

        if (cpu_idle) {
            log_gantt_entry(running.pid, current_time, current_time + 1);
            running.executed_time++;

            if (check_and_start_io(&running, current_time, &io_q, io_events, num_io_events)) {
                cpu_idle = 0;
            } else if (running.executed_time >= running.burst_time) {
                running.end_time = current_time + 1;
                running.turnaround_time = running.end_time - running.arrival_time;
                completed++;
                cpu_idle = 0;
            }
        }

        current_time++;
    }

    calculate_times(processes, n);
}


void run_round_robin_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {
    print_title("Round Robin with I/O");

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);
    // [1] 프로세스를 arrival_time 기준으로 정렬
    sort_by_arrival_time(processes, n);

    int remaining_burst[n], current_time = 0, completed = 0;
    for (int i = 0; i < n; i++) {
        remaining_burst[i] = processes[i].burst_time;
        processes[i].executed_time = 0;
    }

    int next_arrival_idx = 0;
    const int TIME_QUANTUM = 2;

    // [2] 초기 프로세스가 아무도 안 왔을 경우 current_time을 조정
    if (current_time < processes[0].arrival_time)
    current_time = processes[0].arrival_time;

    // [3] 초기 프로세스 도착 처리
    while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time){
        // printf("ENQUEUE RR: pid=%d arrival=%d priority=%d\n", processes[next_arrival_idx].pid, processes[next_arrival_idx].arrival_time, processes[next_arrival_idx].priority);
        enqueue(&ready_q, processes[next_arrival_idx++]);
    }
    // [4] RR
    while (completed < n) {
        process_io_completion(&io_q, &ready_q, current_time);

        if (is_empty(&ready_q)) {
            process_io_completion(&io_q, &ready_q, current_time); // ready_q가 비어 있어도, process_io_completion()체크
            current_time++;
            continue;
        }

        Process p = dequeue(&ready_q);
        int idx = p.pid - 1;
        int exec_time = (remaining_burst[idx] > TIME_QUANTUM) ? TIME_QUANTUM : remaining_burst[idx];

        for (int t = 0; t < exec_time; t++) {
            log_gantt_entry(p.pid, current_time, current_time + 1);
            processes[idx].executed_time++;
            remaining_burst[idx]--;
            current_time++;

            process_io_completion(&io_q, &ready_q, current_time);

            if (check_and_start_io(&processes[idx], current_time - 1, &io_q, io_events, num_io_events))
                goto RR_CONTINUE;

            if (remaining_burst[idx] == 0) {
                processes[idx].end_time = current_time;
                processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
                completed++;
                goto RR_CONTINUE;
            }
        }

        enqueue(&ready_q, processes[idx]);

        RR_CONTINUE:
        while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time)
            enqueue(&ready_q, processes[next_arrival_idx++]);
    }

    calculate_times(processes, n);
}

void run_sjf_preemptive_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {    // 똑같이 짧은 job이면 queue내부에서 앞에 있는 순서가 우선 -> 추후 starvation 방지 추가 예정
    print_title("SJF Preemptive with I/O");

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int current_time = 0, completed = 0;
    int remaining_burst[n];
    bool arrived[n];
    for (int i = 0; i < n; i++) {
        remaining_burst[i] = processes[i].burst_time;
        processes[i].executed_time = 0;
        arrived[i] = false;
    }
    if (current_time < processes[0].arrival_time)
    current_time = processes[0].arrival_time;

    while (completed < n) {
        check_new_arrivals(processes, n, current_time, arrived, &ready_q);
        process_io_completion(&io_q, &ready_q, current_time);

        if (is_empty(&ready_q)) {
            current_time++;
            continue;
        }

        int size = queue_size(&ready_q), min_idx = -1, min_remain = 1e9;
        for (int i = 0; i < size; i++) {
            Process p = dequeue(&ready_q);
            int idx = p.pid - 1;
            if (remaining_burst[idx] > 0 && remaining_burst[idx] < min_remain) {
                min_remain = remaining_burst[idx];
                min_idx = idx;
            }
            enqueue(&ready_q, p);
        }

        if (min_idx != -1) {
            Process *p = &processes[min_idx];
            log_gantt_entry(p->pid, current_time, current_time + 1);
            p->executed_time++;
            remaining_burst[min_idx]--;

            if (check_and_start_io(p, current_time, &io_q, io_events, num_io_events)) {
                current_time++;
                continue;
            }

            if (remaining_burst[min_idx] == 0) {
                p->end_time = current_time + 1;
                p->turnaround_time = p->end_time - p->arrival_time;
                completed++;
            } else {
                enqueue(&ready_q, *p);
            }
        }

        current_time++;
    }

    calculate_times(processes, n);
}

void run_priority_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {  // 같은 priority면 먼저 도착한 순서(Queue 내부 앞쪽)
    print_title("Priority (Non-preemptive) with I/O");

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int current_time = 0, completed = 0;
    bool arrived[n];
    for (int i = 0; i < n; i++) {
        arrived[i] = false;
        processes[i].executed_time = 0;
    }

    Process running;
    int cpu_idle = 0;

    if (current_time < processes[0].arrival_time)
    current_time = processes[0].arrival_time;

    while (completed < n) {
        check_new_arrivals(processes, n, current_time, arrived, &ready_q);
        process_io_completion(&io_q, &ready_q, current_time);

        if (!cpu_idle && !is_empty(&ready_q)) {
            int size = queue_size(&ready_q);
            int best_priority = 1e9;
            Process best_proc;

            for (int i = 0; i < size; i++) {
                Process p = dequeue(&ready_q);
                if (p.priority < best_priority) {
                    best_priority = p.priority;
                    best_proc = p;
                }
                enqueue(&ready_q, p);
            }

            for (int i = 0; i < size; i++) {
                Process p = dequeue(&ready_q);
                if (p.pid == best_proc.pid)
                    running = p;
                else
                    enqueue(&ready_q, p);
            }

            cpu_idle = 1;
            if (running.executed_time == 0)
                running.start_time = current_time;
        }

        if (cpu_idle) {
            log_gantt_entry(running.pid, current_time, current_time + 1);
            running.executed_time++;

            if (check_and_start_io(&running, current_time, &io_q, io_events, num_io_events)) {
                cpu_idle = 0;
            } else if (running.executed_time >= running.burst_time) {
                running.end_time = current_time + 1;
                running.turnaround_time = running.end_time - running.arrival_time;
                completed++;
                cpu_idle = 0;
            }
        }

        current_time++;
    }

    calculate_times(processes, n);
}

void run_priority_preemptive_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {   // 같은 priority인 프로세스가 여럿일 경우, queue에서 먼저 꺼낸 게 아니라 나중에 나오는 프로세스로 덮어씌워질 가능성 있음 -> FCFS 보장 안됨 -> 개선예정
    print_title("Priority Preemptive with I/O");

    Queue ready_q, io_q;
    init_queue(&ready_q);
    init_queue(&io_q);

    int current_time = 0, completed = 0;
    int remaining_burst[n];
    bool arrived[n];
    for (int i = 0; i < n; i++) {
        remaining_burst[i] = processes[i].burst_time;
        processes[i].executed_time = 0;
        arrived[i] = false;
    }
    if (current_time < processes[0].arrival_time)
    current_time = processes[0].arrival_time;


    while (completed < n) {
        check_new_arrivals(processes, n, current_time, arrived, &ready_q);
        process_io_completion(&io_q, &ready_q, current_time);

        if (is_empty(&ready_q)) {
            current_time++;
            continue;
        }

        int size = queue_size(&ready_q);
        int idx = -1, best_priority = 1e9;
        for (int i = 0; i < size; i++) {
            Process p = dequeue(&ready_q);
            int pi = p.pid - 1;
            if (remaining_burst[pi] > 0 && processes[pi].priority < best_priority) {
                best_priority = processes[pi].priority;
                idx = pi;
            }
            enqueue(&ready_q, p);
        }

        if (idx != -1) {
            execute_preemptive_step_with_io(
                processes, idx, 1,
                remaining_burst, &current_time,
                &ready_q, &io_q, &completed,
                io_events, num_io_events
            );
        }
    }

    calculate_times(processes, n);
}









// void run_fcfs_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events) {
//     Queue ready_q, io_q;
//     init_queue(&ready_q);
//     init_queue(&io_q);

//     int current_time = 0;
//     int completed = 0;
//     int cpu_idle = 0; // Process* running = NULL; 로 했을 때 에러가 떠서 대안으로 이 방식을 택하기로 함..
//     Process running;

//     // [1] 도착 시간 기준 정렬 후 초기 ready 큐에 등록
//     sort_by_arrival_time(processes, num_processes);
//     for (int i = 0; i < num_processes; i++) {
//         enqueue(&ready_q, processes[i]);
//     }

//     printf("=== FCFS Scheduling with I/O Simulation ===\n\n");

//     while (completed < num_processes) {
//         // [2] I/O 완료된 프로세스 ready 큐로 복귀 처리
//         process_io_completion(&io_q, &ready_q, current_time);

//         // [3] CPU 할당 (idle 상태 && ready 큐에 대기 중인 프로세스 존재 시)
//         if (!cpu_idle && !is_empty(&ready_q)) {
//             running = dequeue(&ready_q);
//             cpu_idle = 1;
//             printf("DEBUG: [DISPATCH] pid=%d assigned to CPU\n", running.pid);
//             if (running.executed_time == 0)
//                 running.start_time = current_time;
//         }

//         // [4] 실행 중인 프로세스 동작
//         if (cpu_idle) {
//             log_gantt_entry(running.pid, current_time, current_time + 1);
//             running.executed_time++;

//             // [4-1] I/O 요청 발생 여부 확인 → 요청 시 io_q로 이동
//             if (check_and_start_io(&running, current_time, &io_q, io_events, num_io_events)) {
//                 cpu_idle = 0;  // CPU 비움
//             }
//             // [4-2] 완료 조건 확인 (I/O가 발생하지 않았고, Burst 완료 시)
//             else if (running.executed_time >= running.burst_time) {
//                 running.end_time = current_time + 1;
//                 running.turnaround_time = running.end_time - running.arrival_time;
//                 completed++;
//                 cpu_idle = 0;
//                 printf("DEBUG: [COMPLETE] pid=%d at time=%d\n", running.pid, current_time);
//             }
//         }

//         // [5] 시간 증가 및 상태 출력
//         current_time++;
//         printf("DEBUG: time=%d, completed=%d, ready=%d, running_flag=%d\n",
//             current_time, completed, !is_empty(&ready_q), cpu_idle);
//     }

//     // [6] 최종 대기/반환 시간 계산
//     calculate_times(processes, num_processes);
// }

// void run_round_robin_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events) {
//     Queue ready_q, io_q;
//     init_queue(&ready_q);
//     init_queue(&io_q);

//     sort_by_arrival_time(processes, num_processes);

//     int remaining_burst[num_processes];
//     for (int i = 0; i < num_processes; i++)
//         remaining_burst[i] = processes[i].burst_time;

//     int current_time = 0;
//     int completed = 0;
//     int next_arrival_idx = 0;
//     const int TIME_QUANTUM = 2;

//     // 초기 도착 프로세스 삽입
//     while (next_arrival_idx < num_processes && processes[next_arrival_idx].arrival_time <= current_time)
//         enqueue(&ready_q, processes[next_arrival_idx++]);

//     while (completed < num_processes) {
//         // 1. 매 tick마다 I/O 복귀 처리
//         process_io_completion(&io_q, &ready_q, current_time);

//         // 2. 실행 가능한 프로세스가 없으면 시간 진행
//         if (is_empty(&ready_q)) {
//             current_time++;
//             continue;
//         }

//         // 3. ready 큐에서 프로세스 하나 꺼내기
//         Process p = dequeue(&ready_q);
//         int idx = p.pid - 1;  // 원본 배열 접근을 위한 인덱스

//         // 4. 타임퀀텀만큼 실행 (단, burst보다 클 수 없음)
//         int exec_time = (remaining_burst[idx] > TIME_QUANTUM) ? TIME_QUANTUM : remaining_burst[idx];

//         for (int t = 0; t < exec_time; t++) {
//             log_gantt_entry(processes[idx].pid, current_time, current_time + 1);

//             processes[idx].executed_time++;
//             remaining_burst[idx]--;
//             current_time++;

//             // 5. I/O 발생 시 즉시 I/O 큐로 이동
//             if (check_and_start_io(&processes[idx], current_time - 1, &io_q, io_events, num_io_events)) {
//                 goto RR_CONTINUE;
//             }

//             // 6. 프로세스 종료 시 처리
//             if (remaining_burst[idx] == 0) {
//                 processes[idx].end_time = current_time;
//                 processes[idx].turnaround_time = current_time - processes[idx].arrival_time;
//                 completed++;
//                 goto RR_CONTINUE;
//             }
//         }

//         // 7. 아직 종료되지 않은 프로세스는 다시 ready 큐로
//         enqueue(&ready_q, processes[idx]);

//         RR_CONTINUE:
//         // 8. 새로운 도착 프로세스 반영
//         while (next_arrival_idx < num_processes && processes[next_arrival_idx].arrival_time <= current_time)
//             enqueue(&ready_q, processes[next_arrival_idx++]);
//     }
// }

// void run_sjf_preemptive_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {
//     print_title("SJF Preemptive with I/O");

//     int current_time = 0;
//     int completed = 0;

//     int remaining_burst[n];
//     bool arrived[n];
//     for (int i = 0; i < n; i++) {
//         remaining_burst[i] = processes[i].burst_time;
//         arrived[i] = false;
//         processes[i].executed_time = 0;
//     }

//     Queue ready_q, io_q;
//     init_queue(&ready_q);
//     init_queue(&io_q);

//     while (completed < n) {
//         // [1] 도착한 프로세스를 ready_q에 삽입
//         for (int i = 0; i < n; i++) {
//             if (!arrived[i] && processes[i].arrival_time <= current_time) {
//                 enqueue(&ready_q, processes[i]);
//                 arrived[i] = true;
//                 printf("DEBUG: New arrival -> PID=%d at time=%d\n", processes[i].pid, current_time);
//             }
//         }

//         // [2] I/O 완료된 프로세스 복귀
//         process_io_completion(&io_q, &ready_q, current_time);

//         // [3] ready_q에서 남은 burst가 가장 짧은 프로세스 선택
//         int size = queue_size(&ready_q);
//         int min_idx = -1;
//         int min_remain = 1e9;

//         for (int i = 0; i < size; i++) {
//             Process p = dequeue(&ready_q);
//             int idx = p.pid - 1;

//             if (remaining_burst[idx] < min_remain && remaining_burst[idx] > 0) {
//                 min_remain = remaining_burst[idx];
//                 min_idx = idx;
//             }

//             // 큐 유지
//             enqueue(&ready_q, p);
//         }

//         if (min_idx != -1) {
//             Process *p = &processes[min_idx];

//             // Gantt 기록
//             log_gantt_entry(p->pid, current_time, current_time + 1);

//             // 최초 실행 시 시작 시간 기록
//             if (p->executed_time == 0)
//                 p->start_time = current_time;

//             p->executed_time++;
//             remaining_burst[min_idx]--;

//             // I/O 발생 시
//             if (check_and_start_io(p, current_time, &io_q, io_events, num_io_events)) {
//                 current_time++;
//                 continue;
//             }

//             // 종료 시
//             if (remaining_burst[min_idx] == 0) {
//                 p->end_time = current_time + 1;
//                 p->turnaround_time = p->end_time - p->arrival_time;
//                 completed++;
//             } else {
//                 // 아직 안끝났으면 다시 ready_q에 삽입
//                 enqueue(&ready_q, *p);
//             }
//         }

//         current_time++;
//     }

//     calculate_times(processes, n);
// }

// void run_priority_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {
//     print_title("Priority (Non-preemptive) with I/O");

//     Queue ready_q, io_q;
//     init_queue(&ready_q);
//     init_queue(&io_q);

//     int current_time = 0;
//     int completed = 0;
//     int arrived[n];
//     for (int i = 0; i < n; i++) {
//         arrived[i] = 0;
//         processes[i].executed_time = 0;
//     }

//     Process running;
//     int cpu_idle = 0;

//     sort_by_arrival_time(processes, n);  // 초기 도착 기준 정렬

//     while (completed < n) {
//         // 도착한 프로세스 큐에 넣기
//         for (int i = 0; i < n; i++) {
//             if (!arrived[i] && processes[i].arrival_time <= current_time) {
//                 enqueue(&ready_q, processes[i]);
//                 arrived[i] = 1;
//                 printf("DEBUG: check_new_arrivals - PID=%d name=%s\n", processes[i].pid, processes[i].name);
//             }
//         }

//         // I/O 완료된 프로세스 복귀
//         process_io_completion(&io_q, &ready_q, current_time);

//         // CPU가 비어있고 ready 큐에 프로세스가 있으면
//         if (!cpu_idle && !is_empty(&ready_q)) {
//             // 가장 높은 priority의 프로세스 선택
//             int size = queue_size(&ready_q);
//             int best_priority = 1e9;
//             Process best_proc;

//             for (int i = 0; i < size; i++) {
//                 Process p = dequeue(&ready_q);
//                 if (p.priority < best_priority) {
//                     best_priority = p.priority;
//                     best_proc = p;
//                 }
//                 enqueue(&ready_q, p);  // 복원
//             }

//             // 다시 한 번 dequeue해서 가장 높은 priority 꺼냄
//             for (int i = 0; i < size; i++) {
//                 Process p = dequeue(&ready_q);
//                 if (p.pid == best_proc.pid) {
//                     running = p;
//                 } else {
//                     enqueue(&ready_q, p);
//                 }
//             }

//             cpu_idle = 1;
//             if (running.executed_time == 0)
//                 running.start_time = current_time;

//             printf("DEBUG: [DISPATCH] pid=%d assigned to CPU\n", running.pid);
//         }

//         // 실행 중이면 1 tick 수행
//         if (cpu_idle) {
//             log_gantt_entry(running.pid, current_time, current_time + 1);
//             running.executed_time++;

//             // I/O 발생
//             if (check_and_start_io(&running, current_time, &io_q, io_events, num_io_events)) {
//                 cpu_idle = 0;
//             }
//             // 완료
//             else if (running.executed_time >= running.burst_time) {
//                 running.end_time = current_time + 1;
//                 running.turnaround_time = running.end_time - running.arrival_time;
//                 completed++;
//                 cpu_idle = 0;
//                 printf("DEBUG: [COMPLETE] pid=%d at time=%d\n", running.pid, current_time);
//             }
//         }

//         current_time++;
//     }

//     calculate_times(processes, n);
// }


// void run_priority_preemptive_with_io(Process processes[], int n, IOEvent *io_events, int num_io_events) {
//     print_title("Priority Preemptive with I/O");

//     Queue ready_q, io_q;
//     init_queue(&ready_q);
//     init_queue(&io_q);

//     int current_time = 0;
//     int completed = 0;
//     int remaining_burst[n];
//     bool arrived[n];

//     for (int i = 0; i < n; i++) {
//         remaining_burst[i] = processes[i].burst_time;
//         processes[i].executed_time = 0;
//         arrived[i] = false;
//     }

//     while (completed < n) {
//         // 1. 도착 + I/O 복귀 처리
//         for (int i = 0; i < n; i++) {
//             if (!arrived[i] && processes[i].arrival_time <= current_time) {
//                 enqueue(&ready_q, processes[i]);
//                 arrived[i] = true;
//                 printf("DEBUG: New arrival -> PID=%d at time=%d\n", processes[i].pid, current_time);
//             }
//         }
//         process_io_completion(&io_q, &ready_q, current_time);

//         if (is_empty(&ready_q)) {
//             current_time++;
//             continue;
//         }

//         // 2. 현재 ready_q에서 우선순위 가장 높은 프로세스 선택
//         int size = queue_size(&ready_q);
//         int idx = -1;
//         int best_priority = 1e9;

//         for (int i = 0; i < size; i++) {
//             Process p = dequeue(&ready_q);
//             int pi = p.pid - 1;

//             if (remaining_burst[pi] > 0 && processes[pi].priority < best_priority) {
//                 best_priority = processes[pi].priority;
//                 idx = pi;
//             }

//             enqueue(&ready_q, p);  // 다시 큐로 복원
//         }

//         if (idx != -1) {
//             // 1 tick 실행 (선점형)
//             execute_preemptive_step_with_io(
//                 processes, idx, 1,
//                 remaining_burst, &current_time,
//                 &ready_q, &io_q, &completed,
//                 io_events, num_io_events
//             );
//         }
//     }

//     free_queue(&ready_q);
//     free_queue(&io_q);
// }






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

