//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"

void test_queue();

int main(){

    test_queue();

    // printf("===FCFS Scheduling Simulation===\n\n");

    // run_fcfs(scenario_processes, NUM_PROCESSES);
    // print_gantt_chart(scenario_processes, NUM_PROCESSES);
    // print_result_table(scenario_processes, NUM_PROCESSES);

    return 0;
}

#include "queue.h"


// 간단한 큐 테스트용 함수
void test_queue() {
    Queue q;
    init_queue(&q);

    Process p1 = {1, "init", 0, 3, 0, 0, 0, 0};
    Process p2 = {2, "ssh", 1, 4, 0, 0, 0, 0};
    Process p3 = {3, "code", 2, 5, 0, 0, 0, 0};

    printf("=== Queue Test ===\n");

    enqueue(&q, p1);
    enqueue(&q, p2);
    enqueue(&q, p3);

    while (!is_empty(&q)) {
        Process p = dequeue(&q);
        printf("Dequeued: PID=%d, Name=%s, Arrival=%d, Burst=%d\n",
               p.pid, p.name, p.arrival_time, p.burst_time);
    }

    free_queue(&q); // 사실상 dequeue로 비워지지만 습관적으로
}
