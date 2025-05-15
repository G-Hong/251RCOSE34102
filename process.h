//Process 구조 및 데이터 정의
#ifndef PROCESS_H
#define PROCESS_H

typedef struct{
    int pid;
    char name[16];
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int io_request_time;
    int io_burst_time;
}Process;

typedef struct {
    int pid;
    int start_time;
    int end_time;
} GanttEntry;

#define MAX_LOG 1000

//외부 파일에서 정의된 전역변수를 이 파일에서도 사용할 수 있게 선언
extern GanttEntry gantt_log[MAX_LOG];
extern int gantt_log_index;
extern Process scenario_processes[];
extern const int NUM_PROCESSES;

#endif