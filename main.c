//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "scheduler_utils.h"

int main(){

    reset_gantt_log();
    
    print_title("Priority Preemptive Scheduling");
    run_priority_preemptive(scenario_processes, NUM_PROCESSES);
    print_gantt_chart(scenario_processes, NUM_PROCESSES);
    print_result_table(scenario_processes, NUM_PROCESSES);
    
    return 0;
}