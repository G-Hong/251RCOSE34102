//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "scheduler_utils.h"
#include "io_log.h"


int main(){
    reset_gantt_log();
    run_fcfs_with_io(scenario_processes, NUM_PROCESSES);
    print_gantt_chart(scenario_processes, NUM_PROCESSES);
    print_result_table(scenario_processes, NUM_PROCESSES);
    print_io_log();
    return 0;
}