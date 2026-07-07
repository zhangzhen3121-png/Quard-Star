#include "assert.h"
#include "os.h"

static void spin(char* name){
    printk("spinning in %s ...\r\n",name);
    while(1);
}


void assert_failure(char* exp, char* file, char* base, int line){
    printk("\n-->assert(%s) failed\n",exp);
    printk("-->file: %s\n",file);
    printk("-->base: %s\n",base);
    printk("-->line: %d\n",line);
    spin("assertion_failure()");
}