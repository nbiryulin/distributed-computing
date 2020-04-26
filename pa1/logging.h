//
// Created by maxifier on 08.04.2020.
//

#ifndef PA1_STARTER_CODE_LOGGING_H
#define PA1_STARTER_CODE_LOGGING_H


#include "common.h"
#include <stdio.h>
#include "pa2345.h"
#include <sys/types.h>
#include <unistd.h>
#include "ipc.h"

FILE * events_fp;
FILE * pipe_fp;

void log_msg(Message *const message, const char *format, ...);

void log_begin();

/**
 * Информацию обо всех открытых дескрипторах каналов (чтение / запись)
 * необходимо вывести в файл pipes.log. Это помогает обнаружить часто встречаемую
 * ошибку: реализацию топологии «общая шина» вместо полносвязной. Кроме того, следует
 * не забывать, что неиспользуемые дескрипторы необходимо закрыть.
 */


void log_format(const char *format, ...);

void log_fd_r_open(int fd);

void log_fd_w_open(int fd);

void log_fd_closed(int fd);

void log_done();

void log_start();

void log_recd();


void log_recs();

void log_end();


#endif //PA1_STARTER_CODE_LOGGING_H
