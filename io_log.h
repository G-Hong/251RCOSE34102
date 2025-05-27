// io_log.h
#include "process.h"

#ifndef IO_LOG_H
#define IO_LOG_H

#define MAX_IO_LOG 1000

typedef struct {
    int time;           // 현재 시간
    int pid;            // 프로세스 ID
    char action[16];    // "IO Start" or "IO Done"
    int trigger_time;   // I/O 발생 트리거 시간
    int duration;       // I/O 수행 시간
} IOLogEntry;

// I/O 로그 저장
void log_io_event(int time, int pid, const char* action, int trigger_time, int duration);

// 로그 출력
void print_io_log();
void reset_io_log();
void clear_io_reqs(Process processes[], int n);


extern IOLogEntry io_log[];
extern int io_log_index;

#endif