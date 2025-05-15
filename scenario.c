#include "process.h"

Process scenario_processes[] = {
    {1, "init",     0, 2, 0, 0, 0, 0},
    {2, "zsh",      1, 1, 0, 0, 0, 0},
    {3, "firefox",  2, 8, 0, 0, 0, 0},
    {4, "code",     3, 6, 0, 0, 0, 0},
    {5, "sshd",     4, 3, 0, 0, 0, 0},
    {6, "apt",      5, 7, 0, 0, 0, 0}
};

const int NUM_PROCESSES = sizeof(scenario_processes) / sizeof(Process);