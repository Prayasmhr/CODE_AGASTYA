#ifndef _HT_UNIQUE_PTR_H
#define _HT_UNIQUE_PTR_H

#include"ht_defines.h"

/*
    caution this is very experimental, and it is not completed yet --.
*/

typedef struct UniquePtr _UniquePtr;

_UniquePtr* unique_ptr_alloc(u64 size);
void unique_ptr_move(_UniquePtr* dest,const _UniquePtr* src);
void unique_ptr_copy(_UniquePtr* dest,const _UniquePtr* src);
u64 unique_ptr_cmp(const _UniquePtr* dest,const _UniquePtr* src);
void unique_ptr_free(_UniquePtr* block);

#define unique_ptr(type)  
#endif
