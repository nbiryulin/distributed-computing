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

int cp_count;

int main(int argc, char *argv[]) {


    //input
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        cp_count = atoi(argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    //array of pids

    /**
     * Каждый процесс должен иметь свой локальный идентификатор: [0..N−1]. Причем
     * родительскому процессу присваивается идентификатор PARENT_ID, равный 0. Данные
     * идентификаторы используются при отправке и получении сообщений.
     * **/


    pid_t pids[cp_count];
    pids[PARENT_ID] = getpid();
    printf("%d\n",pids[0]);

    /**При запуске программы родительский процесс осуществляет необходимую
     * подготовку для организации межпроцессного взаимодействия, после чего создает X
     * идентичных дочерних процессов. Функция родительского процесса ограничивается
     * созданием дочерних процессов и дальнейшим мониторингом их работы.
     * **/
        local_id l_id;
    for (int i = 1; i <= cp_count; i++) {
        int cp_id = fork();
        if (cp_id > 0) {
            //in parent
            l_id = PARENT_ID;
            pids[i] = cp_id;
        } else if ( cp_id == 0){
            //a new process
            l_id=i;
            break;
        }
    }
    log_begin();


    if(l_id == 0) {
        for (int j = 0; j <= cp_count; j++) {
            printf("%d\n", pids[j]);
            log_start();
        }
    }
    log_end();
}



