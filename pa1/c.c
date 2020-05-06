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
        printf("begin of child while %d \n", c->id);
        Message m;
        receive_any(c, &m);
        increase_l_time(c, m.s_header.s_local_time);
        MessageType m_type = m.s_header.s_type;
        switch (m_type) {
            case STOP:
                printf(" %d  receive stop \n", c->id);
                stop = 0;
                //todo check for time?
                printf(" %d sending done \n", c->id);
                send_done(c);
                printf(" %d done sent \n", c->id);
                break;
            case TRANSFER:
                printf(" %d  begin transfer \n", c->id);
                do_transfer(c, &m);
                printf(" %d end transfer \n", c->id);
                break;
            case DONE:
                printf(" %d  receive done \n", c->id);
                left--;
                break;
            default:
                fprintf(stderr, "Wrong type of message\n");
                return 1;
        }
    }
    log_format(log_received_all_done_fmt, get_lamport_time(), c->id);
    send_balance_history(c);
    return 0;
}

static int do_transfer(p *c, Message *m) {
    BalanceHistory *history = &c->history;
    TransferOrder *order = (TransferOrder *) &(m->s_payload);
    timestamp_t time = m->s_header.s_local_time;
    if (order->s_src == c->id) {
        //todo do we need to inc time?
        increase_l_time(c, time);
        timestamp_t my_time = get_lamport_time();
        for (timestamp_t fortime = my_time; fortime <= MAX_T; fortime++) {
            printf(" %d  in for \n", c->id);
            history->s_history[fortime].s_balance -= order->s_amount;
        }

        m->s_header.s_local_time = my_time;
        send(&process_self, order->s_dst, m);
        printf("Source %s sending type %d", &process_self.id, m->s_header.s_type);
        log_format(log_transfer_out_fmt, my_time, c->id, order->s_amount, order->s_dst);
    } else if (order->s_dst == c->id) {
        //todo do we need to inc time?
        increase_l_time(c, time);
        timestamp_t my_time = get_lamport_time();
        for (timestamp_t  fortime = time; fortime < my_time; fortime++) {
            history->s_history[fortime].s_balance_pending_in += order->s_amount;
        }
        for (timestamp_t  fortime = my_time; fortime <= MAX_T; fortime++) {
            history->s_history[fortime].s_balance += order->s_amount;
        }

        Message ack_message;
        ack_message.s_header = (MessageHeader) {
                .s_type = ACK,
                .s_payload_len = 0,
                .s_magic = MESSAGE_MAGIC,
                .s_local_time = get_lamport_time(),
        };
        printf(" %d  sending type %d \n", c->id, ack_message.s_header.s_type);
        log_format(log_transfer_in_fmt, get_lamport_time(), c->id, order->s_amount, order->s_src);
        send(&process_self, PARENT_ID, &ack_message);
    } else {
        fprintf(stderr, "wrong dst");
    }
    return 0;
}


