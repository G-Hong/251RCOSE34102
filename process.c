//현실적인 시나리오
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "process.h"

//리눅스 부팅 후 사용자가 firefox를 열고 vscode에서 작업하는 상황
Process scenario_processes[] = {
    {1, "init",     0, 2, 1, 0, 0, 0, 0, 0, 0, 0},  // 0초 컴퓨터 켬, init 실행
    {2, "zsh",      1, 1, 3, 0, 0, 0, 0, 0, 0, 0},  // 1초 터미널 실행, zsh 셸
    {3, "firefox",  2, 8, 5, 0, 0, 0, 0, 0, 0, 0},  // 2초 웹 브라우저 열기, firefox
    {4, "code",     3, 6, 4, 0, 0, 0, 0, 0, 0, 0},  // 3초 코드 편집 시작, code(vs code)
    {5, "sshd",     4, 3, 2, 0, 0, 0, 0, 0, 0, 0},  // 백그라운드 ssh 접속 발생(sshd)
    {6, "apt",      5, 7, 6, 0, 0, 0, 0, 0, 0, 0}   // 시스템 업데이트(apt)
};

const int NUM_PROCESSES = sizeof(scenario_processes) / sizeof(Process);

// 랜덤 생성된 프로세스 저장용 전역 배열
Process random_scenario_processes[10];
const int RANDOM_NUM_PROCESSES = 10;



// 프로세스 수: n
// 도착 시간: 0 ~ 10초 사이
// CPU burst: 5 ~ 15
// priority: 1 (높음) ~ n (낮음)
// I/O 발생 횟수: 0~2회 (중간 시점만 허용)
// I/O duration: 1~4

void generate_processes(Process processes[], int n) {
    const char *names_pool[] = {
        "init", "zsh", "firefox", "code", "sshd",
        "apt", "nano", "top", "chrome", "vim"
    };
    int total_names = sizeof(names_pool) / sizeof(names_pool[0]);
    int used_indices[10] = {0}; // 이름 중복 방지

    if (n > total_names) {
        printf("Error: n(%d) exceeds number of unique names available (%d)\n", n, total_names);
        exit(1);
    }

    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        // 중복되지 않는 이름 인덱스 선택
        int name_idx;
        do {
            name_idx = rand() % total_names;
        } while (used_indices[name_idx]);
        used_indices[name_idx] = 1;

        processes[i].pid = i + 1;
        snprintf(processes[i].name, sizeof(processes[i].name), "%s", names_pool[name_idx]);
        processes[i].arrival_time = rand() % 11;               // 0 ~ 10
        processes[i].burst_time = 5 + rand() % 11;              // 5 ~ 15
        processes[i].priority = 1 + rand() % n;                 // 1 ~ n
        processes[i].remaining_time = processes[i].burst_time;

        // 초기화
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].executed_time = 0;
        processes[i].io_complete_time = 0;
        processes[i].start_time = 0;
        processes[i].end_time = 0;
    }
}
