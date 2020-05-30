cmake_minimum_required (VERSION 2.6)
project (pa1_starter_code)
include_directories(..)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -pedantic")

add_executable(peer
        logging.h
        ipc.c
        common.h
        ipc.h
        helper.h
        logging.c
        c.h
        c.c
        k.h
        k.c
        banking.h
        bank_robbery.c
        pa23.c
        pa2345.h
        io.c
        io.h
        banking.c
        lamport.c
        )
