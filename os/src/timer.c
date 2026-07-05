#include "os.h"
#include "sbi.h"

#define MHZ     1000000

#define CPU_FEQ  10*MHZ
#define us_CLICK CPU_FEQ/MHZ   
#define ms_CLICK 1000*us_CLICK

static reg_t task_t_ms;

void time_irq_enable(){
    reg_t sie = r_sie();
    sie |= (0x1<<5);
    w_sie(sie);
}

void time_irq_disable(){
    reg_t sie = r_sie();
    sie &= ~(0x1<<5);
    w_sie(sie);
}


void timer_init(reg_t t_ms){

    reg_t sstatus = r_sstatus();
    sstatus |= (0x1<<1);
    w_sstatus(sstatus);
    time_irq_enable();

    task_t_ms = t_ms;

    reg_t mtimer = r_mtime();
    mtimer += task_t_ms*ms_CLICK;
    sbi_set_time(mtimer);
}


void set_next_trigger(){
    reg_t mtimer = r_mtime();
    mtimer += task_t_ms*ms_CLICK;
    sbi_set_time(mtimer);
}