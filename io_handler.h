//I/O 요청 처리 및 큐 상태 업데이트 전담
#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "io_event_data.h"
#include "process.h"
#include "queue.h"

// 실행 중인 프로세스가 I/O 이벤트 발생 조건을 만족하면 I/O 큐로 이동시키는 함수
void handle_io_request(Process **running, int current_time, Queue *io_q, IOEvent *io_events, int n);

// I/O 큐에 있는 프로세스들의 상태 업데이트 및 완료된 프로세스를 ready 큐로 복귀시키는 함수
void update_io_queue(int current_time, Queue *io_q, Queue *ready_q);

// 매 tick마다 전체 I/O 처리 흐름을 담당하는 통합 함수
void process_io_events(Process **running, int current_time, Queue *io_q, Queue *ready_q, IOEvent *io_events, int n);

void process_io_completion(Queue *io_q, Queue *ready_q, int current_time);
int check_and_start_io(Process *running, int current_time, Queue *io_q, IOEvent *io_events, int n);


#endif