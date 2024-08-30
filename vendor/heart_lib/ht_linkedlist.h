#ifndef _HT_LINKEDLIST_H
#define _HT_LINKEDLIST_H

#include"ht_defines.h"
#include"stdio.h"

typedef struct ll_node {
    void* data;
    struct ll_node* next;
} ll_node;

typedef struct linked_list {
    u64 size;
    u64 node_size;
    ll_node* head;
    ll_node* curr;
} linked_list;

linked_list* linkedlist_create(u64 node_size);
bool linkedlist_insert_head(linked_list* self, const void* data);
bool linkedlist_insert(linked_list* self, const void* data);
bool linkedlist_search(const linked_list* self, const void* value, i32 (*compare)(const void*, const void*));
bool linkedlist_delete_position(linked_list* self, u64 position);
bool linkedlist_delete(linked_list* self, const void* value, i32 (*compare)(const void*, const void*));
void linkedlist_destroy(linked_list* self, void (*destroy)(void*));
void* linkedlist_get(const linked_list* self, u64 position);
u64 linkedlist_size(const linked_list* self);

#endif
