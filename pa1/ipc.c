//
// Created by maxifier on 09.04.2020.
//




#include <unistd.h>
#include "ipc.h"
#include "helper.h"
#include "logging.h"

static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes);

int send(void *self, local_id dst, const Message *msg) {
    //todo maybe change id
    if (write((fd_w[l_id][dst]), &msg->s_header, sizeof(MessageHeader)) == -1) {
        fprintf(stderr, "Cannot send message\n");
        return 1;
    }
    if (write((fd_w[l_id][dst]), &msg->s_payload, msg->s_header.s_payload_len) == -1) {
        fprintf(stderr, "Cannot send message\n");
        return 1;
    }
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    for (int d = 0; d < (cp_count + 1); d++) {
        if (d != l_id) {
            if (send(self, d, msg) > 0) {
                return 1;
            }
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    //todo handle errors
    read_by_bytes(fd_r[from][l_id], &msg->s_header, sizeof(MessageHeader));
    read_by_bytes(fd_r[from][l_id], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    // TODO: Not implemented yet
    return fprintf(stderr, "Not implemented yet");
}

//todo change variables
static size_t read_by_bytes(size_t fd, void *buf, size_t num_bytes) {
    size_t offset = 0;
    size_t remaining = num_bytes;
    while (remaining > 0) {
        int num_bytes_read = read(fd, ((char *)buf) + offset, remaining);
        if (num_bytes_read > 0) {
            remaining -= num_bytes_read;
            offset += num_bytes_read;
        } else {
            return -1;
        }
    }
    return offset;
}
