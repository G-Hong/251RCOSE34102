//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "scheduler_utils.h"
#include "io_log.h"
#include "io_event_data.h"


int main(){
    reset_gantt_log();
    run_round_robin_with_io(scenario_processes, NUM_PROCESSES, scenario_io_events1, NUM_IO_EVENTS1);
    print_gantt_chart(scenario_processes, NUM_PROCESSES);
    print_result_table(scenario_processes, NUM_PROCESSES);
    print_io_log();
    return 0;
}