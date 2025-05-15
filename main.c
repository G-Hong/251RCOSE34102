//전체 흐름, 메인함수
#include <stdio.h>
#include "process.h"
#include "scheduler.h"

int main(){
    printf("===FCFS Scheduling Simulation===\n\n");

    run_fcfs(scenario_processes, NUM_PROCESSES);
    print_gantt_chart(scenario_processes, NUM_PROCESSES);
    print_result_table(scenario_processes, NUM_PROCESSES);

    return 0;
}