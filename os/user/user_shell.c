#include "string.h"
#include "os.h"
#define LF 0x0a
#define DL 0x7f  //back
#define CR 0x0d  //enter
#define BS 0x80  //backspace

#define BUFFERSIZE 1024

int main(){

    char cmd[BUFFERSIZE];
    int len = 0;
    while(1){
        char c = getchar();
        
        if(c!=LF && c!=BS && c!=CR && c!=DL){
            printf("%c",c);
            cmd[len] = c;
            len++;
        }
        // else{
        //     printf("%x\n",c);
        // }
        else if(c == CR){
            if(len>0){
                cmd[len] = '\0';
                printf("\0shell: %s \n", cmd);
                if(!sys_exec(cmd)){
                    printf("\nno %s command \n",cmd);
                    len = 0;
                }
            }
            else{
                len = 0;
                printf("\n");
            }
        }
        else if(c == DL){
            if(len>0){
                printf("\b \b");
                cmd[len] = '\0';
                len--;
            }
        } 
    }

}