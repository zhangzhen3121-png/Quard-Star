#include "os.h"

extern size_t fork();

int main(){

    size_t subpid = fork();
    while (1)
    {   
        int i = 10000;
        while(i--){
            int j = 10000;
            while (j--);     
        }
        if(subpid > 0) printf("this timer father process \n");
        else printf("this timer child process \n");
    }
    
    return 0;
}