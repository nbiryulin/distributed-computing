//
// Created by maxifier on 13.05.2020.
//

/*
/всякий раз, когда процесс собирается войти в КС, он помещает
свой запрос вместе с отметкой времени в свою локальную очередь и
рассылает сообщение с этим же запросом всем остальным процессам. При
получении запроса остальные процессы помещают его уже в свои
локальные очереди. По порядку обслуживания запросов процесс получит
право войти в КС, когда значение отметки времени его запроса окажется
наименьшим среди всех других запросов
 */

#include "helper.h"
#include "stdio.h"

int request_cs(const void *self) {
    //printf("in request cs\n");
    p *process = ((p *) self);
    process->l_time++;
    //todo do we need to inc time if it increments weile sending
    queue_put(&process->l_queue, (element) {.id=process->id, .time = get_lamport_time(),
    });

    Message message = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_local_time = get_lamport_time(),
                    .s_type = CS_REQUEST,
                    .s_payload_len = 0,
            },
            //todo payload?
    };

    send_multicast_child(process, &message);
//int asd =0;
 //   printf("before while\n");
    int left = cp_count - 1 ;
    while (left > 0 || queue_peek(&process->l_queue).id != process->id) {

        //if(left == 0 ){
    //        printf("peeek for %d id %d \n", process->id, queue_peek(&process->l_queue).id);
    //        printf("prcoess %d size %d \n", process->id, process->l_queue.size);

     //   }
//        if(process->id == 3){
//            printf("elements size %d \n", process->l_queue.size);
//        }
       //
        //printf("%d in while\n", process->id);
        Message r_message;
        local_id r_id = receive_any(process, &r_message);
//        printf("%d got message \n", process->id);

        increase_l_time(process, r_message.s_header.s_local_time);
        printf("process %d got from %d size is %d  and peek %d  and left %d and type %hd \n", process->id, r_id, process->l_queue.size, queue_peek(&process->l_queue).id, left, message.s_header.s_type);
        switch (r_message.s_header.s_type) {
            case CS_REQUEST:
                //printf("%d get cs request \n" , process->id);
                queue_put(&process->l_queue, (element) {
                        .id = r_id,
                        .time = r_message.s_header.s_local_time,
                });
                process->l_time++;
                Message s_message = {
                        .s_header = {
                                .s_local_time = get_lamport_time(),
                                .s_type = CS_REPLY,
                                .s_payload_len = 0,
                                .s_magic = MESSAGE_MAGIC,
                        }
                };
                send(process, r_id, &s_message);
             //   printf("process %d send reply to %d \n", process->id, r_id);
                break;
            case CS_REPLY:
//                if(process->id == 1){
//                    printf("process 1 got reply"){
//
//                    }
//                }
          //      printf("process %d got reply from %d \n ", process->id, r_id);
                //todo inc time? or do we need to increment it after message?
                printf("process %d received reply from %d \n", process->id, r_id);
                left--;
                break;
            case CS_RELEASE:
                queue_pop(&process->l_queue);
                printf(" pr %d received release from %d \n", process->id, r_id);
//                if(process->id == 1){
//                    for (int i = 0; i < process->l_queue.size ; ++i) {
//                        printf("%d \n", process->l_queue.elements->id);
//                    }
//                }
left--;
                break;
            case DONE:
                process->done++;
 //               queue_pop(&process->l_queue);
//                if (process->l_queue.size == 0) {
//                    queue_put(&process->l_queue, (element) {.id=process->id, .time = get_lamport_time(),
//                    });
//                }
                printf("process %d peerk %d \n", process->id, queue_peek(&process->l_queue).id);
                break;
            default:
                fprintf(stderr, "ERROR\n");
                break;
        }
    }
//    printf("after while \n");
    return 0;
}


void queue_put(queue *self, element element){
    self->elements[self->size] = element;
    self->size++;
}

void queue_pop(queue *self){
    //todo maybe delete element by id
   // if (self->size <= 0) {
  //      queue_put(self, (element) {.id=process->id, .time = get_lamport_time(),
    //    });)
  //      return ;
  //  }
    int id = get_first(self);

//    element a = self->elements[id];
 //   if(id !=0 ) {
        self->elements[id] = self->elements[self->size-1];
   // }
    self->size--;
}

element queue_peek(queue *self){
    int id = get_first(self);
  //  printf("id peek %d \n",self->elements[id].id);
    return self->elements[id];
}
int cmp(element e, element max);

int get_first(queue *q){
    //todo this shit may not work
//    if (q->size <= 0) {
//        printf("size is 0 \n");
//        return 0;
//    }
//    int id = 0;
//    for (int i = 1; i < q->size; ++i) {
//        if (cmp(q->elements[i], q->elements[id]) > 0) {
//            id = i;
//        }
//    }
//    return id;
int id = 0;
int pid = 0;
int time = 1422121;
//printf("size is %d \n" , q->size);
for(int i = 0; i <q->size; i++){
   // printf("i is %d \n" , i);
    if(q->elements[i].time < time){
        id = i;
        time = q->elements[i].time;
        pid = q->elements[i].id;
    } else if(time == q->elements[i].time){
        if(pid < q->elements[i].id){
            pid =  q->elements[i].id;
            time = q->elements[i].time;
            id = i;
        }
    }
   // printf("size is %d \n" , q->size);
}
 //   printf("id is %d \n" , id);
    return id;
}

//int cmp(element e, element max) {
//    if (e.time > max.time) {
//        return 1;
//    } else if (e.time < max.time) {
//        return -1;
//    } else {
//        if (e.id > max.id) {
//            return 1;
//        } else if (e.id < max.id) {
//            return -1;
//        } else {
//            return 0;
//        }
//    }
//}

int release_cs(const void *self) {
    p *process = ((p *) self);
    if(process->id == 3){
        printf("process %d poped %d \n", process->id, queue_peek(&process->l_queue).id);
    }

    queue_pop(&process->l_queue);
    if(process->id == 3){
        printf("process %d peer after pop %d \n", process->id, queue_peek(&process->l_queue).id);
    }

    process->l_time++;
    Message message = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_payload_len = 0,
                    .s_type = CS_RELEASE,
                    .s_local_time = get_lamport_time(),
            },
    };
    send_multicast_child(process, &message);
    if(process->id == 3){
        printf("process %d send multicast \n", process->id);
    }
    return 0;
}
