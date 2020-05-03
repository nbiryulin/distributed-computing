//
// Created by maxifier on 26.04.2020.
//

#include <wait.h>
#include "k.h"
#include "io.h"
#include "banking.h"

void run_k(p *k) {

    receive_started(k);

    bank_robbery(k, cp_count);

    send_stop(k);

    receive_done(k);

    receive_balance_history(k);
}
