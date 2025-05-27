// io_log.c
#include <stdio.h>
#include <string.h>
#include "io_log.h"
#include "process.h"

IOLogEntry io_log[MAX_IO_LOG];
int io_log_index = 0;

void reset_io_log() {
    io_log_index = 0;
}

void clear_io_reqs(Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        processes[i].io_reqs_str[0] = '\0';
    }
}

void log_io_event(int time, int pid, const char* action, int trigger_time, int duration) {
    if (io_log_index >= MAX_IO_LOG) return;

    io_log[io_log_index].time = time;
    io_log[io_log_index].pid = pid;
    strncpy(io_log[io_log_index].action, action, sizeof(io_log[io_log_index].action) - 1);
    io_log[io_log_index].action[sizeof(io_log[io_log_index].action) - 1] = '\0';
    io_log[io_log_index].trigger_time = trigger_time;
    io_log[io_log_index].duration = duration;
    io_log_index++;
}

void print_io_log() {
    printf("\n[IO Log]\n");
    printf("Time\tPID\tAction\t\tTrigger\tDuration\n");
    for (int i = 0; i < io_log_index; i++) {
        printf("%d\t%d\t%-10s\t",
            io_log[i].time,
            io_log[i].pid,
            io_log[i].action);

        if (strcmp(io_log[i].action, "IO Start") == 0)
            printf("%d\t%d\n", io_log[i].trigger_time, io_log[i].duration);
        else
            printf("-\t-\n");
    }
}