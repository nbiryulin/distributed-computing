//
// Created by maxifier on 08.04.2020.
//

#ifndef PA1_STARTER_CODE_LOGGING_H
#define PA1_STARTER_CODE_LOGGING_H


#include "common.h"
#include <stdio.h>
#include "pa1.h"
#include <sys/types.h>
#include <unistd.h>
#include "ipc.h"

FILE * fp;
FILE * pipe_fp;
local_id l_id;


void log_begin(){
    fp = fopen(events_log,"w");
    pipe_fp = fopen(pipes_log, "w");
}

/**
 * Информацию обо всех открытых дескрипторах каналов (чтение / запись)
 * необходимо вывести в файл pipes.log. Это помогает обнаружить часто встречаемую
 * ошибку: реализацию топологии «общая шина» вместо полносвязной. Кроме того, следует
 * не забывать, что неиспользуемые дескрипторы необходимо закрыть.
 */

void log_fd_r_open(int fd){
    fprintf(pipe_fp, "fd %d opened for reading\n", fd);
}

void log_fd_w_open(int fd){
    fprintf(pipe_fp, "fd %d opened for writing\n", fd);
}

void log_fd_closed(int fd){
    fprintf(pipe_fp, "fd %d closed\n", fd);
}

void log_done(){
    fprintf(fp, log_done_fmt, l_id);
}

void log_start(){
    pid_t pid = getpid();
    pid_t ppid = getppid();
    fprintf(fp, log_started_fmt, l_id, pid , ppid);
}

void log_recd(){
    fprintf(fp, log_received_all_done_fmt, l_id);
}


void log_recs(){
    fprintf(fp, log_received_all_started_fmt, l_id);
}

void log_end(){
    fclose(fp);
    fclose(pipe_fp);
}


#endif //PA1_STARTER_CODE_LOGGING_H
