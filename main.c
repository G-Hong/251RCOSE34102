//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "scheduler_utils.h"

int main(){

    reset_gantt_log();
    run_fcfs_with_io(scenario_processes, NUM_PROCESSES);
    return 0;
    
    return 0;
}