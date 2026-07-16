#include "os.h"

void delay(volatile int count){
    count*=50000;
    while(count--);
}

void task_1(){
    
    while (1)
    {
        printf("this is task 1 \r\n");
        delay(10000);
        // sys_yield();
    }
    
}

void task_2(){

    while (1)
    {
        printf("this is task 2 \r\n");
        delay(10000);
        // sys_yield();
    }
}

void task_3(){
   
    while (1)
    {
        printf("this is task 3 \r\n");
        delay(10000);
        // sys_yield();
    }
}

void task_init(){
    task_creat(task_1);
    task_creat(task_2);
    task_creat(task_3);
}
