#include"ht_unique_ptr.h"
#include"stdlib.h"

struct UniquePtr {
    void* block;
    u64 block_size;
    _UniquePtr* next;
};

void unique_ptr_init(_UniquePtr* self,u64 size,void* block) {
    self->block_size = size;
    
}

void unique_ptr_move(_UniquePtr* dest,const _UniquePtr* src) {
    free(src->block);
}

void unique_ptr_copy(_UniquePtr* dest,const _UniquePtr* src) {
	
}
u64 unique_ptr_cmp(const _UniquePtr* dest,const _UniquePtr* src) {
    return 0;
}
void unique_ptr_free(_UniquePtr* block) {
	
}
