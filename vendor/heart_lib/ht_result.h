#ifndef _HT_RESULT_H
#define _HT_RESULT_H

#include"ht_defines.h"

typedef struct result {
    enum {
        SUCCESS,
        FAILURE,
    } type;
    union {
        char* err_msg;
        i32 err_code;
        void* handle;
    } data;
} result;

#endif
