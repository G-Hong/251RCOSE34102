//시뮬레이터 실행
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "scheduler_utils.h"
#include "io_log.h"
#include "io_event_data.h"

void generate_full_scenario(Process processes[], int num, IOEvent io_events[], int *num_io_events);
void total_algorithm_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events);

int main() {

    // 아래 두 줄은 잘 작동
    // generate_full_scenario(random_scenario_processes, RANDOM_NUM_PROCESSES, random_scenario_io_events, &RANDOM_NUM_IO_EVENTS);
    // total_algorithm_with_io(random_scenario_processes, RANDOM_NUM_PROCESSES, random_scenario_io_events, RANDOM_NUM_IO_EVENTS);
    total_algorithm_with_io(scenario_processes, NUM_PROCESSES, scenario_io_events, NUM_IO_EVENTS);
    // total_algorithm_with_io(scenario_processes, NUM_PROCESSES, random_scenario_io_events, RANDOM_NUM_IO_EVENTS);
    // clear_io_reqs(scenario_processes, NUM_PROCESSES);
    // reset_io_log();
    // reset_gantt_log();
    // run_round_robin_with_io(scenario_processes, NUM_PROCESSES, scenario_io_events, NUM_IO_EVENTS);
    // print_gantt_chart(scenario_processes, NUM_PROCESSES);
    // print_result_table(scenario_processes, NUM_PROCESSES);
    // print_io_log();
    

    return 0;
}




void generate_full_scenario(Process processes[], int num, IOEvent io_events[], int *num_io_events) {
    generate_processes(processes, num);
    generate_safe_io_events(processes, num, io_events, num_io_events);
}


void total_algorithm_with_io(Process processes[], int num_processes, IOEvent io_events[], int num_io_events) {
    clear_io_reqs(processes, num_processes);
    reset_io_log();
    reset_gantt_log();
    run_fcfs_with_io(processes, num_processes, io_events, num_io_events);
    print_gantt_chart(processes, num_processes);
    print_result_table(processes, num_processes);
    print_io_log();

    clear_io_reqs(processes, num_processes);
    reset_io_log();
    reset_gantt_log();
    run_round_robin_with_io(processes, num_processes, io_events, num_io_events);
    print_gantt_chart(processes, num_processes);
    print_result_table(processes, num_processes);
    print_io_log();

    clear_io_reqs(processes, num_processes);
    reset_io_log();
    reset_gantt_log();
    run_sjf_preemptive_with_io(processes, num_processes, io_events, num_io_events);
    print_gantt_chart(processes, num_processes);
    print_result_table(processes, num_processes);
    print_io_log();

    clear_io_reqs(processes, num_processes);
    reset_io_log();
    reset_gantt_log();
    run_priority_with_io(processes, num_processes, io_events, num_io_events);
    print_gantt_chart(processes, num_processes);
    print_result_table(processes, num_processes);
    print_io_log();

    clear_io_reqs(processes, num_processes);
    reset_io_log();
    reset_gantt_log();
    run_priority_preemptive_with_io(processes, num_processes, io_events, num_io_events);
    print_gantt_chart(processes, num_processes);
    print_result_table(processes, num_processes);
    print_io_log();
}
