#ifndef __ASSERT_H
#define __ASSERT_H


void assert_failure(char* exp, char* file, char* base, int line);


#define assert(exp)\
    if(exp)\
    ;      \
    else   \
    assert_failure(#exp,__FILE__,__BASE_FILE__,__LINE__);


#endif