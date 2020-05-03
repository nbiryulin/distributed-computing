//
// Created by maxifier on 09.04.2020.
//




#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ipc.h"
#include "helper.h"
#include "logging.h"

static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes);

int send(void *self, local_id dst, const Message *msg) {
    p *process = self;
    if (write((fd_w[process->id][dst]), &msg->s_header, sizeof(MessageHeader)) == -1) {
        fprintf(stderr, "Cannot send message\n");
        return 1;
    }
    if (write((fd_w[process->id][dst]), &msg->s_payload, msg->s_header.s_payload_len) == -1) {
        fprintf(stderr, "Cannot send message\n");
        return 1;
    }
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    p *process = self;
    for (int d = 0; d < (cp_count + 1); d++) {
        if (d != process->id) {
            if (send(self, d, msg) > 0) {
                return 1;
            }
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    p *process = self;
    read_by_bytes(fd_r[from][process->id], &msg->s_header, sizeof(MessageHeader));
    read_by_bytes(fd_r[from][process->id], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    p *process = (p *) self;
    while(1) {
        int from = process->id + 1;
        if (from > cp_count) {
            from = from - cp_count - 1;
        }
        int fd = fd_r[from][process->id];
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        //https://www.geeksforgeeks.org/non-blocking-io-with-pipes-in-c/
        int nread = read(fd, &msg->s_header, 1);
        switch (nread) {
            case -1:

                // case -1 means pipe is empty and errono
                // set EAGAIN
                if (errno == EAGAIN) {
                    sleep(1);
                    continue;
                } else {
                    perror("read");
                }

                // case 0 means all bytes are read and EOF(end of conv.)
            case 0:
                sleep(1);
                continue;
            default:
                // text read
                // by default return no. of bytes
                // which read call read at that time
                break;
        }

        fcntl(fd, F_SETFL, flags & !O_NONBLOCK);
        read(fd, ((char *) &msg->s_header) + 1, sizeof(MessageHeader) - 1);
        read(fd, msg->s_payload, msg->s_header.s_payload_len);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        return 0;
    }
}


    static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags & !O_NONBLOCK);
        size_t already = 0;
        size_t yet = num_bytes;
        while (yet > 0) {
            int num_bytes_read = read(fd, ((char *) buf) + already, yet);
            if (num_bytes_read > 0) {
                yet -= num_bytes_read;
                already += num_bytes_read;
            } else {
                return -1;
            }
        }
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        return already;
    }
