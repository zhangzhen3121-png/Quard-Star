#ifndef __STACK_H
#define __STACK_H

#include "types.h"

#define STACK_MAX  1000



typedef struct {
    uint64_t data[STACK_MAX];
    int _top;
}Stack;


void Stack_init(Stack* stack);
bool is_empty(Stack* stack);
bool is_full(Stack* Stack);
void push(Stack* stack, uint64_t value);
uint64_t pop(Stack* stack);
uint64_t top(Stack* stack);





#endif