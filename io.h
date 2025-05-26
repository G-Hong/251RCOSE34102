// 내부 처리 함수 시그니처만 정의
#ifndef IO_H
#define IO_H

#include "process.h"
#include "queue.h"
//TODO
//I/O 발생 요청 처리 모듈
//io 요청이 발생하면 프로세스를 waiting queue로 이동시키고 io 시간이 끝나면 다시 ready queue로 복귀시키는 역할

// 실행 중인 프로세스가 현재 I/O 이벤트에 해당하면 I/O 큐로 이동
void handle_io_request(Process **running_process, int current_time, Queue *io_queue);

// I/O 큐 내 프로세스들의 상태 업데이트 (io_remaining_time--)
void update_io_queue(Queue *io_queue, Queue *ready_queue);

// I/O가 끝났는지 검사하는 유틸 (내부용)
int is_io_done(Process *p);

// 한 tick마다 I/O 흐름 전체 처리 (통합 함수)
void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q);

#endif
