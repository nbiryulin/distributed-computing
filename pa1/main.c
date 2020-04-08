//
// Created by maxifier on 29.03.2020.
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "ipc.h"
#include <stdlib.h>
#include "logging.h"

#define MAX_PROCESS 10


int cp_count;
local_id l_id;
int fd_r[MAX_PROCESS][MAX_PROCESS];
int fd_w[MAX_PROCESS][MAX_PROCESS];


int main(int argc, char *argv[]) {
    log_begin();

    //input
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        cp_count = atoi(argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    for (int in = 0; in <= cp_count; in++) {
        for (int out = 0; out <= cp_count; out++) {
            int fd[2];
            if (pipe(fd) == 0) {
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

    /**
     * Каждый процесс должен иметь свой локальный идентификатор: [0..N−1]. Причем
     * родительскому процессу присваивается идентификатор PARENT_ID, равный 0. Данные
     * идентификаторы используются при отправке и получении сообщений.
     * **/


    pid_t pids[cp_count];
    l_id = PARENT_ID;
    pids[PARENT_ID] = getpid();
    printf("%d\n", pids[0]);

    /**При запуске программы родительский процесс осуществляет необходимую
     * подготовку для организации межпроцессного взаимодействия, после чего создает X
     * идентичных дочерних процессов. Функция родительского процесса ограничивается
     * созданием дочерних процессов и дальнейшим мониторингом их работы.
     * **/

    for (int i = 1; i <= cp_count; i++) {
        int cp_id = fork();
        if (cp_id > 0) {
            //in parent
            l_id = PARENT_ID;
            pids[i] = cp_id;
        } else if (cp_id == 0) {
            //a new process
            l_id = i;
            break;
        }
    }


    for (int in = 0; in <= cp_count; in++) {
        for (int out = 0; out <= cp_count; out++) {
            if(out != l_id) {
                //maybe there error
                log_fd_closed(fd_r[in][out]);
                close(fd_r[in][out]);
            }
            if(in != l_id){
                log_fd_closed(fd_w[in][out]);
                close(fd_w[in][out]);
            }
        }
    }


    if (l_id == 0) {
        for (int j = 0; j <= cp_count; j++) {
            printf("%d\n", pids[j]);
            log_start();
        }
    }


    log_end();
    return 0;
}



