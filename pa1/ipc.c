//
// Created by maxifier on 09.04.2020.
//



#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ipc.h"
#include "helper.h"
#include "logging.h"


static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes, int try);

int send(void *self, local_id dst, const Message *msg) {
    p *process = self;
    process->l_time++;
    if (dst > cp_count) {
        fprintf(stderr, "Destionation is more than count of c processes %d \n", dst);
    }
    if (write((fd_w[process->id][dst]), &msg->s_header, sizeof(MessageHeader)) == -1) {
        fprintf(stderr, "Cannot send message\n");
        return 1;
    } else if (write((fd_w[process->id][dst]), &msg->s_payload, msg->s_header.s_payload_len) == -1) {
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
    read_by_bytes(fd_r[from][process->id], &msg->s_header, sizeof(MessageHeader), 0);
    read_by_bytes(fd_r[from][process->id], &msg->s_payload, msg->s_header.s_payload_len, 0);
    return 0;
}

int receive_any(void *self, Message *msg) {
    p *process = (p *) self;
    printf(" %d begin receive any \n", process->id);
    int from = process->id;
    while (1) {
        from = from + 1;
        if (from == process->id) {
            from++;
        }
        if (from > cp_count) {
            from = from - cp_count - 1;
        }
        //    printf("begin recieve any while %d from %d \n", process->id, from);
        int fd = fd_r[from][process->id];
        //https://www.geeksforgeeks.org/non-blocking-io-with-pipes-in-c/
        int nread = read_by_bytes(fd, &msg->s_header, sizeof(MessageHeader), 1);
        switch (nread) {
            case -1:
                // case -1 means pipe is empty and errono
                // set EAGAIN
                if (errno == EAGAIN) {
                    //           sleep(1);
                    //  usleep( 100000 );
                    continue;
                } else {
                    perror("read");
                    return -1;
                }

                // case 0 means all bytes are read and EOF(end of conv.)
            case 0:
                //           sleep(1);
                //    usleep( 100000 );
                continue;
            default:
                // text read
                // by default return no. of bytes
                // which read call read at that time
                read_by_bytes(fd, msg->s_payload, msg->s_header.s_payload_len, 0);
                return 0;
        }
    }
}


static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes, int try) {
    size_t already = 0;
    size_t yet = num_bytes;
    if (try) {
        int n_bytes = read(fd, ((char *) buf) + already, yet);
        if (n_bytes > 0) {
            yet -= n_bytes;
            already += n_bytes;
        } else {
            return n_bytes;
        }
    }
    while (yet > 0) {
        int n_bytes = read(fd, ((char *) buf) + already, yet);
        if (n_bytes > 0) {
            yet -= n_bytes;
            already += n_bytes;
        } else if (n_bytes == -1 && errno == EAGAIN) {
            continue;
        } else {
            return n_bytes;
        }
    }
    return already;
}
