#ifndef _HT_RESULT_H
#define _HT_RESULT_H

#include"ht_defines.h"

/*
 I know this thing is not used anywhere but i will use it later, especially in the tjlang part
*/

typedef struct result {
    enum {
        SUCCESS,
        FAILURE,
    };
    union {
        char* err_msg;
        i32 err_code;
    } data;
} result;

#endif
