//외부 I/O 이벤트 시나리오 정의만 따로
//반드시 h 파일도 추가, 수정 해주기***
#include <stdlib.h>
#include <time.h>
#include "io_event_data.h"
#include "process.h"


IOEvent scenario_io_events[] = {
    {3, 1, 4, 0},
    {3, 2, 3, 0},
    {4, 4, 1, 0},
    {4, 5, 3, 0}
};

// IOEvent scenario_io_events[] = {
//     {1, 9,  2, 0},
//     {1, 10, 2, 0},
//     {2, 6,  1, 0},
//     {2, 7,  3, 0},
//     {4, 2,  3, 0},
//     {4, 3,  2, 0},
//     {5, 1,  2, 0},
//     {5, 9,  4, 0},
//     {6, 3,  3, 0},
//     {6, 8,  2, 0},
//     {7, 1,  1, 0},
//     {7, 3,  3, 0},
//     {8, 2,  4, 0},
//     {8, 5,  1, 0},
//     {9, 2,  1, 0},
//     {9, 10, 1, 0},
//     {10, 4, 1, 0}
// };


const int NUM_IO_EVENTS = sizeof(scenario_io_events) / sizeof(IOEvent);

IOEvent random_scenario_io_events[30];
int RANDOM_NUM_IO_EVENTS = 0;


void generate_safe_io_events(const Process processes[], int n, IOEvent io_events[], int *io_event_count) {
    *io_event_count = 0;
    srand(time(NULL) + 1);  // process 생성과 시드 충돌 방지

    for (int i = 0; i < n; i++) {
        int burst = processes[i].burst_time;
        if (burst <= 2) continue; // 너무 짧은 프로세스는 제외

        int io_count = rand() % 3; // 각 프로세스당 최대 2개의 I/O
        int last_io_end = -1;

        for (int j = 0; j < io_count; j++) {
            int trigger, duration;
            int attempt = 0;

            do {
                trigger = 1 + rand() % (burst - 1);  // 1 ~ burst-1
                duration = 1 + rand() % 3;           // 1 ~ 3
                attempt++;
            } while (((trigger <= last_io_end) || (trigger + duration > burst)) && attempt < 10);

            if (attempt >= 10) break;  // 너무 많은 충돌 시 패스

            last_io_end = trigger + duration;

            io_events[*io_event_count].pid = processes[i].pid;
            io_events[*io_event_count].trigger_time = trigger;
            io_events[*io_event_count].duration = duration;
            io_events[*io_event_count].handled = 0;
            (*io_event_count)++;
        }
    }
}








// void generate_io_events(const Process processes[], int n, IOEvent io_events[], int *io_event_count) {
//     *io_event_count = 0;
//     srand(time(NULL) + 1);  // process 생성과 시드 충돌 방지

//     for (int i = 0; i < n; i++) {
//         int burst = processes[i].burst_time;

//         if (burst <= 2) continue; // 너무 짧으면 I/O 넣지 않음

//         int io_count = rand() % 3; // 0~2회

//         for (int j = 0; j < io_count; j++) {
//             int trigger = 1 + rand() % (burst - 2);  // 1 ~ burst-2
//             int duration = 1 + rand() % 4;           // 1 ~ 4

//             io_events[*io_event_count].pid = processes[i].pid;
//             io_events[*io_event_count].trigger_time = trigger;
//             io_events[*io_event_count].duration = duration;
//             io_events[*io_event_count].handled = 0;
//             (*io_event_count)++;
//         }
//     }
// }
