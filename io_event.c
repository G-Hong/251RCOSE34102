// 시나리오 정의
#include "io_event.h"

IOEvent io_events[] = {
    {3, 1, 4},  // 3ms에 PID 1 I/O 요청, 4ms 소요
    {6, 3, 2},  // 6ms에 PID 3 I/O 요청, 2ms 소요
    {8, 1, 3},  // 8ms에 PID 1 또 요청, 3ms 소요
};

const int NUM_IO_EVENTS = sizeof(io_events) / sizeof(IOEvent);
