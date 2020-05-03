//
// Created by maxifier on 26.04.2020.
//

#include <stdio.h>
#include "helper.h"
#include "io.h"
#include "logging.h"

static int do_transfer(p *c, Message *m);

int run_c(p *c, balance_t balance) {

    c->history.s_id = c->id;
    c->history.s_history_len = 1;
    for (timestamp_t time = 0; time <= MAX_T; ++time) {
        c->history.s_history[time] = (BalanceState) {
                .s_balance = balance,
                .s_balance_pending_in = 0,
                .s_time = time,
        };
    }

    send_started(c);

    receive_started(c);

    unsigned int stop = 1;

    unsigned int left = cp_count - 1;

    while (stop | left) {
        Message m;
        receive_any(c, &m);
        MessageType m_type = m.s_header.s_type;
        switch (m_type) {
            case STOP:
                stop = 0;
                break;
            case TRANSFER:
                do_transfer(c, &m);
                break;
            case DONE:
                left++;
                break;
            default:
                fprintf(stderr, "Wrong type of message\n");
                return 1;
        }
    }
    send_done(c);
    log_format(log_received_all_done_fmt, get_physical_time(), c->id);
    send_balance_history(c);
    return 0;
}

static int do_transfer(p *c, Message *m) {
    BalanceHistory *history = &c->history;
    balance_t difference = 0;
    TransferOrder *order = (TransferOrder *) &(m->s_payload);
    timestamp_t time = get_physical_time();
    if (order->s_src == c->id) {
        difference -= order->s_amount;
        send(c, order->s_dst, m);
        log_format(log_transfer_out_fmt, get_physical_time(), c->id, order->s_amount, order->s_dst);
    } else {
//todo seems like message cannot else be there but maybe there error
        difference = order->s_amount;
        Message ack_message;
        ack_message.s_header = (MessageHeader) {
                .s_type = ACK,
                .s_payload_len = 0,
                .s_magic = MESSAGE_MAGIC,
                .s_local_time = time
        };
        send(&c, PARENT_ID, &ack_message);
        log_format(log_transfer_in_fmt, get_physical_time(), c->id, order->s_amount, order->s_src);
    }


    for (; time <= MAX_T; time++) {
        history->s_history[time].s_balance += difference;
    }

    if (time >= history->s_history_len) history->s_history_len = time + 1;
    return 0;
}


