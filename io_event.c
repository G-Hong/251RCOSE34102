// 시나리오 정의
#include "io_event.h"

// IOEvent io_events[] = {
//     {3, 1, 4},  // 3ms에 PID 1 I/O 요청, 4ms 소요
//     {6, 3, 2},  // 6ms에 PID 3 I/O 요청, 2ms 소요
//     {8, 1, 3},  // 8ms에 PID 1 또 요청, 3ms 소요
// };

IOEvent io_events[] = {
    {1, 1, 3},  // PID 1 (init) 1ms에 I/O 요청 → 3ms 동안 I/O
    {4, 3, 2},  // PID 3 (firefox) 4ms에 요청 → 2ms 동안 I/O
    {6, 4, 3},  // PID 4 (code) 6ms에 요청 → 3ms 동안 I/O
};


const int NUM_IO_EVENTS = sizeof(io_events) / sizeof(IOEvent);
