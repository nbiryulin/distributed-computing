//
// Created by maxifier on 26.04.2020.
//

#ifndef PA1_STARTER_CODE_IO_H
#define PA1_STARTER_CODE_IO_H

#include "helper.h"

int send_started(p *process);

int receive_started(p *process);

int send_done(p *process);

int receive_done(p *process);

int send_stop(p *process);

int send_balance_history(p *process);

int receive_balance_history(p *process);

#endif //PA1_STARTER_CODE_IO_H
