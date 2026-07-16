#include"load.h"

extern uint64_t _num_app[];

int get_app_num(){
    return (int)_num_app[0];
}

APPMATEDATA get_app_data(int id){
    APPMATEDATA data;
    data.addr = _num_app[id+1];
    data.size = _num_app[id+2]-_num_app[id+1];
    return data;
}