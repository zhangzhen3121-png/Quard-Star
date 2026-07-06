#include"stack.h"

void Stack_init(Stack* stack){
    stack->_top = -1;
}


bool is_empty(Stack* stack){
    return stack->_top<0;
}

bool is_full(Stack* Stack){
    return Stack->_top>(STACK_MAX-1);
}

void push(Stack* stack, uint64_t value){
    if(stack->_top < STACK_MAX-1){
        stack->_top++;
        stack->data[stack->_top] = value;
    }
}


uint64_t pop(Stack* stack){
    uint64_t value;
    if(stack->_top >= 0){
        value = stack->data[stack->_top];
        stack->_top--;
        return value;
    }
    return -1;
}

uint64_t top(Stack* stack){
    uint64_t value;
    if(stack->_top >= 0){
        value = stack->data[stack->_top];
        return value;
    }
    return -1;
}