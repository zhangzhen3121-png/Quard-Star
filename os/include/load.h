#ifndef __LOADER_H
#define __LOADER_H

#include"types.h"

typedef struct 
{
    uint64_t addr;
    uint64_t size;
}APPMATEDATA;

int get_app_num();

APPMATEDATA get_app_data();

#endif