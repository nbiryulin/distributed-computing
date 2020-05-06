//
// Created by maxifier on 03.05.2020.
//

#include "ipc.h"
#include "helper.h"



timestamp_t get_lamport_time(){
    return process_self.l_time;
}

int increase_l_time(p *process, timestamp_t time){
    if(time > process->l_time){
        process->l_time = time;
    }
    process->l_time++;
    return 0;
}

