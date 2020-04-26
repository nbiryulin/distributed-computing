//
// Created by maxifier on 15.04.2020.
//

#ifndef PA1_STARTER_CODE_HELPER_H
#define PA1_STARTER_CODE_HELPER_H

#include "ipc.h"
#include "banking.h"

#define MAX_PROCESS 10

int fd_r[MAX_PROCESS][MAX_PROCESS];
int fd_w[MAX_PROCESS][MAX_PROCESS];
int cp_count;


typedef struct {
    local_id id;
    BalanceHistory history;
    AllHistory all_history;
} p;


// todo delete p process;

balance_t balances[MAX_PROCESS];


#endif //PA1_STARTER_CODE_HELPER_H
