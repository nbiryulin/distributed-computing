#include "banking.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "ipc.h"
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>
#include "logging.h"
#include "helper.h"
#include "k.h"
#include "c.h"


void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    Message msg;
    msg.s_header = (MessageHeader) {
            .s_local_time = get_physical_time(),
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = sizeof(TransferOrder),
            .s_type = TRANSFER
    };
    TransferOrder transferOrder = {
            .s_amount = amount,
            .s_dst = dst,
            .s_src = src
    };
    memcpy(&msg.s_payload, &transferOrder, sizeof(TransferOrder));
    send(parent_data, src, &msg);

    receive(parent_data, dst, &msg);
    if (msg.s_header.s_type != ACK) {
        fprintf(stderr, "Not ACK type\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
        cp_count = atoi(argv[2]);
        for (int i = 1; i <= cp_count; i++) {
            balances[i] = atoi(argv[i + 2]);
        }
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    log_begin();

    for (int in = 0; in <= cp_count; in++) {
        for (int out = 0; out <= cp_count; out++) {
            int fd[2];
            if (in != out) {
                if (pipe(fd) == 0) {
                    /* Добавляем флаг O_NONBLOCK к дескриптору fd */
                    int flags = fcntl(fd[1], F_GETFL, 0);
                    fcntl(fd[1], F_SETFL, flags | O_NONBLOCK);
                    flags = fcntl(fd[0], F_GETFL, 0);
                    fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);
                    log_fd_r_open(fd[0]);
                    log_fd_w_open(fd[1]);
                    fd_r[in][out] = fd[0];
                    fd_w[in][out] = fd[1];
                } else {
                    fprintf(stderr, "Cannot create pipe\n");
                    return 1;
                }
            }
        }
    }

    pid_t pids[cp_count];
    pids[PARENT_ID] = getpid();
    p *process = &process_self;

    for (int i = 1; i <= cp_count; i++) {
        int cp_id = fork();
        if (cp_id > 0) {
            //in parent
            process->id = PARENT_ID;
            pids[i] = cp_id;
        } else if (cp_id == 0) {
            //a new process
            process->id = i;
            break;
        }
    }


    for (int in = 0; in <= cp_count; in++) {
        for (int out = 0; out <= cp_count; out++) {
            if (out != process->id) {
                //maybe there error
                log_fd_closed(fd_r[in][out]);
                close(fd_r[in][out]);
            }
            if (in != process->id) {
                log_fd_closed(fd_w[in][out]);
                close(fd_w[in][out]);
            }
        }
    }

    if (process->id == PARENT_ID) {
        run_k(process);
        for (int i = 1; i <= (cp_count + 1); i++) {
            waitpid(pids[i], NULL, 0);
        }
    } else {
        run_c(process, balances[process->id]);
    }

//todo maybe e there
    print_history(&process->all_history);
    log_end();
    return 0;
}
