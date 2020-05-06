//
// Created by maxifier on 15.04.2020.
//



#include "logging.h"
#include "helper.h"
#include <stdarg.h>


void log_msg(Message *const message, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(events_fp, format, args);
    va_end(args);
    va_start(args, format);
    size_t payload_length = sprintf(message->s_payload, format, args);
    message->s_header.s_payload_len = payload_length;
    va_end(args);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void log_begin() {
    printf("Begin of opening file\n");
    events_fp = fopen(events_log, "w");
    pipe_fp = fopen(pipes_log, "w");
    printf("End of opening file \n");
}


void log_format(const char *format, ...){
    va_list args;
    va_start(args, format);
    vfprintf(events_fp, format, args);
    va_end(args);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}


/**
 * Информацию обо всех открытых дескрипторах каналов (чтение / запись)
 * необходимо вывести в файл pipes.log. Это помогает обнаружить часто встречаемую
 * ошибку: реализацию топологии «общая шина» вместо полносвязной. Кроме того, следует
 * не забывать, что неиспользуемые дескрипторы необходимо закрыть.
 */

void log_fd_r_open(int fd) {
    fprintf(pipe_fp, "fd %d opened for reading\n", fd);
}

void log_fd_w_open(int fd) {
    fprintf(pipe_fp, "fd %d opened for writing\n", fd);
}

void log_fd_closed(int fd) {
    fprintf(pipe_fp, "fd %d closed\n", fd);
}


void log_end() {
    fclose(events_fp);
    fclose(pipe_fp);
}

