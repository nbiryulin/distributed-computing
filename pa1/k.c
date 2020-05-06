//
// Created by maxifier on 26.04.2020.
//

#include <wait.h>
#include "k.h"
#include "io.h"
#include "banking.h"
#include "stdio.h"

void run_k(p *k) {

    receive_started(k);
    printf("begin robbery \n ");
    bank_robbery(k, cp_count);
    printf("end robbery \n");
    send_stop(k);
    printf("end stop \n");
    receive_done(k);
    receive_balance_history(k);
    for (int i = 1; i <= (cp_count + 1); i++) {
        waitpid(pids[i], NULL, 0);
    }
    print_history(&k->all_history);

}
