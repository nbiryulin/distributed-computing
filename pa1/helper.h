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





int mutexl;

//const int multiply = 5;

typedef struct  {
    local_id  id;
    timestamp_t time;
} element;

typedef struct {
    element elements[MAX_PROCESS];
    int size;
} queue;

typedef struct {
    local_id id;
    BalanceHistory history;
     AllHistory all_history;
    timestamp_t l_time;
    queue l_queue;
    int done;
} p;

int increase_l_time(p *process, timestamp_t time);

balance_t balances[MAX_PROCESS];
p process_self;

void queue_put(queue *self, element element);

void queue_pop(queue *self);

element queue_peek(queue *self);

int get_first(queue *q);

int send_multicast_child(void *self, const Message *msg);




#endif //PA1_STARTER_CODE_HELPER_H
