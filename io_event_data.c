//외부 I/O 이벤트 시나리오 정의만 따로
//반드시 h 파일도 추가, 수정 해주기***
#include "io_event_data.h"

IOEvent scenario_io_events[] = {
    {3, 2, 2, 0},
    {4, 1, 2, 0},
    {4, 4, 1, 0},
    {6, 3, 2, 0}
};

const int NUM_IO_EVENTS = sizeof(scenario_io_events) / sizeof(IOEvent);
