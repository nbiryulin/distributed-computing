//
// Created by maxifier on 26.04.2020.
//

#include <unistd.h>
#include "io.h"
#include "ipc.h"
#include "pa2345.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"
#include "logging.h"


int send_started(p *process) {
    //todo fix
    Message message = {.s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_type = STARTED,
    },
    };
    timestamp_t time = get_physical_time();
    log_msg(&message, log_started_fmt, time, process->id, getpid(), getppid(), balances[process->id])
    message.s_header.s_payload_len = strlen(message.s_payload);
    //todo maybe e there
    return send_multicast(process, &message);
}

int receive_started(p *process) {
    for (int i = 1; i <= cp_count; i++) {
        Message message;
        if (i == process->id) {
            continue;
        }
        //todo maybe e there
        if (receive(process, i, &message) == 1) {
            return 1;
        }
    }
    timestamp_t time = get_physical_time();
    log_format(log_received_all_started_fmt, time, process->id);
    return 0;
}

int send_done(p *process) {
    Message message = {.s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_type = DONE,
    },
    };
    timestamp_t time = get_physical_time();
    log_msg(&message, log_done_fmt, time, process->id, process->history.s_history[time].s_balance);
    message.s_header.s_payload_len = strlen(message.s_payload);
    //todo maybe e there
    return send_multicast(process, &message);
}

int receive_done(p *process){
    for (int i = 1; i <= cp_count; i++) {
        if (i == process->id) {
            continue;
        }
        Message message;
        if( receive(process, i, &message) == 1 ){
            return 1;
        }
    }
    timestamp_t time = get_physical_time();
    log_format(log_received_all_done_fmt, time, process->id);
    return 0;
}

int send_stop(p *process){
    Message message = {
            .s_header =
                    {
                            .s_magic = MESSAGE_MAGIC,
                            .s_type = STOP,
                            .s_payload_len = 0,
                            .s_local_time = get_physical_time(),
                    },
    };
    //todo maybe e therre
    return send_multicast(process, &message);
}

int send_balance_history(p *process){
    process->history.s_history_len = get_physical_time() + 1;
    size_t size_of_history = sizeof(local_id) +
                             sizeof(uint8_t) +
                             process->history.s_history_len * sizeof(BalanceState);

    Message message = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = BALANCE_HISTORY,
                    .s_local_time = get_physical_time(),
                    .s_payload_len = size_of_history,
            }
    };
    memcpy(&message.s_payload, &process->history, size_of_history);
   //todo maybe e there
    return send(process, PARENT_ID, &message);
}

int receive_balance_history(p *process){
    process->all_history.s_history_len = cp_count;
    for (size_t c_id = 1; c_id <= cp_count; c_id++) {
        Message message;
        //todo wtf DEBUG("Awaiting BALANCE_HISTORY from %lu\n", c_id);
        //todo maybe e therre &
        receive(process, c_id, &message);
     //   int16_t msg_type = message.s_header.s_type;
//        if (msg_type != BALANCE_HISTORY) {
//            fprintf(stderr,
//                    "NOTICE: Expected message of type %d (BALANCE_HISTORY), "
//                    "got %d instead\n", BALANCE_HISTORY, msg_type);
//        } else {
            //todo is this needed?
            //todo find out and rewrite this shit method
            BalanceHistory *their_history = (BalanceHistory *) &message.s_payload;
            process->all_history.s_history[c_id - 1] = *their_history;
       // }
    }
}

