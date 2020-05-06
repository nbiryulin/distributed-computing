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
    printf("%d transfer to %d", src, dst);
    p *process = parent_data;
    process->l_time++;
    Message msg;
    msg.s_header = (MessageHeader) {
            .s_local_time = get_lamport_time(),
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
        fprintf(stderr, "Not ACK type %d \n", msg.s_header.s_type);
    } else {
        increase_l_time(process, msg.s_header.s_local_time);
    }
    printf("%d transfer end %d", src, dst);
}

int main(int argc, char *argv[]) {
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
        printf("Begin \n");
        cp_count = strtol(argv[2], NULL, 10);
        for (int i = 1; i <= cp_count; i++) {
            balances[i] = strtol(argv[2 + i], NULL, 10);
        }
        printf("End\n");
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    log_begin();

    for (int in = 0; in <= cp_count; in++) {
        for (int out = 0; out <= cp_count; out++) {
            print("Begin of creating pipes\n");
            int fd[2];
            if (in != out) {
                if (pipe(fd) == 0) {
                    print("pipe ok \n");
                    /* Добавляем флаг O_NONBLOCK к дескриптору fd */
                    int flags = fcntl(fd[1], F_GETFL, 0);
                    print("a \n");
                    fcntl(fd[1], F_SETFL, flags | O_NONBLOCK);
                    print("B \n");
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


    pids[PARENT_ID] = getpid();
    p *process = &process_self;
    process->l_time = 0;

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
            printf("closing\n");
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
        printf("end k run");

    } else {
        run_c(process, balances[process->id]);
        printf("end c run");
    }

//todo maybe e there

    log_end();
    return 0;
}
