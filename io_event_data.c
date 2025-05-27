//외부 I/O 이벤트 시나리오 정의만 따로
//반드시 h 파일도 추가, 수정 해주기***
#include <stdlib.h>
#include <time.h>
#include "io_event_data.h"
#include "process.h"


IOEvent scenario_io_events[] = {
    {3, 2, 2, 0},
    {4, 1, 2, 0},
    {4, 4, 1, 0},
    {6, 3, 2, 0}
};

const int NUM_IO_EVENTS = sizeof(scenario_io_events) / sizeof(IOEvent);

IOEvent random_scenario_io_events[30];
int RANDOM_NUM_IO_EVENTS = 0;



void generate_io_events(const Process processes[], int n, IOEvent io_events[], int *io_event_count) {
    *io_event_count = 0;
    srand(time(NULL) + 1);  // process 생성과 시드 충돌 방지

    for (int i = 0; i < n; i++) {
        int burst = processes[i].burst_time;

        if (burst <= 2) continue; // 너무 짧으면 I/O 넣지 않음

        int io_count = rand() % 3; // 0~2회

        for (int j = 0; j < io_count; j++) {
            int trigger = 1 + rand() % (burst - 2);  // 1 ~ burst-2
            int duration = 1 + rand() % 4;           // 1 ~ 4

            io_events[*io_event_count].pid = processes[i].pid;
            io_events[*io_event_count].trigger_time = trigger;
            io_events[*io_event_count].duration = duration;
            io_events[*io_event_count].handled = 0;
            (*io_event_count)++;
        }
    }
}
