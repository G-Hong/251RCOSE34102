//현실적인 시나리오
#include "process.h"

//리눅스 부팅 후 사용자가 firefox를 열고 vscode에서 작업하는 상황
Process scenario_processes[] = {
    {1, "init",     0, 2, 1, 0, 0, 0, 0, 0},  // 0초 컴퓨터 켬, init 실행
    {2, "zsh",      1, 1, 3, 0, 0, 0, 0, 0},  // 1초 터미널 실행, zsh 셸
    {3, "firefox",  2, 8, 5, 0, 0, 0, 0, 0},  // 2초 웹 브라우저 열기, firefox
    {4, "code",     3, 6, 4, 0, 0, 0, 0, 0},  // 3초 코드 편집 시작, code(vs code)
    {5, "sshd",     4, 3, 2, 0, 0, 0, 0, 0},  // 백그라운드 ssh 접속 발생(sshd)
    {6, "apt",      5, 7, 6, 0, 0, 0, 0, 0}   // 시스템 업데이트(apt)
};

const int NUM_PROCESSES = sizeof(scenario_processes) / sizeof(Process);