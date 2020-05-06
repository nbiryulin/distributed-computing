//
// Created by maxifier on 15.04.2020.
//

#ifndef PA1_STARTER_CODE_HELPER_H
#define PA1_STARTER_CODE_HELPER_H

#include <sys/types.h>
#include "ipc.h"
#include "banking.h"

#define MAX_PROCESS 10

int fd_r[MAX_PROCESS][MAX_PROCESS];
int fd_w[MAX_PROCESS][MAX_PROCESS];
int cp_count;
pid_t pids[MAX_PROCESS];


typedef struct {
    local_id id;
    BalanceHistory history;
    AllHistory all_history;
    timestamp_t l_time;
} p;

int increase_l_time(p *process, timestamp_t time);

balance_t balances[MAX_PROCESS];
p process_self;

#endif //PA1_STARTER_CODE_HELPER_H
