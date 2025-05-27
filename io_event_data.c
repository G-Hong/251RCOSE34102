//외부 I/O 이벤트 시나리오 정의만 따로
#include "io_event_data.h"

IOEvent io_events[] = {
    {1, 2, 3},
    {1, 2, 1},
    {4, 3, 2},
    {6, 4, 3}
};

const int NUM_IO_EVENTS = sizeof(io_events) / sizeof(IOEvent);
