//Process 구조 및 데이터 정의
#ifndef PROCESS_H
#define PROCESS_H
#define MAX_IO_REQ_STR_LEN 256

typedef struct {
    int pid;
    char name[32];
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;

    int waiting_time;
    int turnaround_time;
    
    int executed_time;      // [추가됨] 누적 실행 시간 (I/O 요청 트리거 판단용)
    int io_complete_time;   // [추가됨] I/O 완료 예상 시간 (복귀 판단용)

    int start_time;
    int end_time;

    char io_reqs_str[MAX_IO_REQ_STR_LEN];  // 🔹추가!
} Process;


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
extern Process random_scenario_processes[];
extern const int RANDOM_NUM_PROCESSES;

void generate_processes(Process processes[], int n);

#endif