#include"os.h"

extern size_t sys_write(size_t fd, const char* buf, size_t size);

static char out_buf[1024];

static size_t sys_printf(const char* buf){
    return sys_write(1, buf, strlen(buf));
}

static int _vsnprintf(char* out, int n, const char* format, va_list args) {
    size_t pos = 0;
    int format_flag = 0;
    int longarg_flag = 0;

    for(;*format;format++){
        if(format_flag){
            switch (*format)
            {
                case 'l':{
                    longarg_flag = 1;
                    break;
                }
                case 'p':{
                    if(pos<n-1 && out){
                            out[pos] = '0';
                            out[pos+1] = 'x';
                    }
                    pos+=2;
                    longarg_flag = 1;
                    break;
                }
                case 'x':{
                    long num = longarg_flag ? va_arg(args, long) : va_arg(args, int);
                    int hexdigit = (longarg_flag ? sizeof(long) * 2 : sizeof(int) * 2) -1;
                    for(int i = hexdigit; i >= 0; i--){
                        int d = (num>>(4*i))&0xF;
                        if(pos<n && out){
                            out[pos] = (char)(d>9?'a'+d-10:'0'+d);
                        }
                        pos++;
                    }

                    format_flag = 0;
                    longarg_flag = 0;

                    break;
                }
                case 'd':{
                    long numd = longarg_flag ? va_arg(args, long) : va_arg(args, int);

                    if(numd<0){
                        if(pos<n && out){
                            out[pos] = '-';
                        }
                        pos++;
                        numd = -numd;
                    }
                    int digit = 1;
                    for(long nn = numd; nn/=10; digit++);
                    for(int i = digit-1;i>=0;i--){
                        if(pos+i<n && out){
                            out[pos+i] = '0'+(numd%10);
                        }

                        numd /= 10;
                    }

                    pos+=digit;
                    format_flag = 0;
                    longarg_flag = 0;
                    break;
                }
                case 's':{
                    char* s = va_arg(args, char*);
                    while (*s)
                    {
                        if(pos<n && out){
                            out[pos] = *s;
                            
                        }
                        pos++;
                        s++;
                    }
                    
                    format_flag = 0;
                    break;
                }
                case 'c':{
                    char ch = (char)va_arg(args,int);
                    if(pos<n && out){
                        out[pos] = ch;
                        
                    }
                    pos++;
                    format_flag = 0;
                    longarg_flag = 0;
                    break;
                }
                default:
                    break;
            }
        }
        else if(*format == '%'){
            format_flag = 1;
        }
        else{
            if(pos < n){
                out[pos] = *format;
            }
            pos++;
        }
    }

    if(pos<n && out){
        out[pos] = '\0';
    }
    else if(pos && out){
        out[n-1] = '\0';
    }

    return pos;
}

static int _vprintf(const char *format, va_list args) {
    va_list copy_args;
    va_copy(copy_args,args);
    int res = _vsnprintf(NULL, -1,format, copy_args);
    va_end(copy_args);
    
    if(res+1 >= sizeof(out_buf)){
        sys_printf("out_buf is overflowed\n");
        return -1;
    }
    _vsnprintf(out_buf, res+1, format, args);
    sys_printf(out_buf);
    return res;
}


int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = _vprintf(format, args);
    va_end(args);
    return ret;
}