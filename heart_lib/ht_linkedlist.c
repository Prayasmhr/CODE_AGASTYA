#include"ht_linkedlist.h"
#include"stdlib.h"
#include"memory.h"

linked_list* linkedlist_create(u64 node_size) {
    linked_list* out = malloc(sizeof(linked_list));
    out->head = NULL;
    out->curr = NULL;
    out->node_size = node_size;
    out->size = 0;
    return out;
}

bool linkedlist_insert_head(linked_list* self, const void* data) {
    if (!self || !data) return 0;
    ll_node* new_node = malloc(sizeof(ll_node));
    if (!new_node) return 0;
    new_node->data = malloc(self->node_size);
    if (!new_node->data) {
        free(new_node);
        return 0;
    }
    memcpy_s(new_node->data, sizeof(new_node->data),data, self->node_size);
    new_node->next = self->head;
    self->head = new_node;
    self->size++;
    return 1;
}

bool linkedlist_insert(linked_list* self, const void* data) {
    if (!self || !data) return 0;
    ll_node* new_node = malloc(sizeof(ll_node));
    if (!new_node) return 0;
    new_node->data = malloc(self->node_size);
    if (!new_node->data) {
        free(new_node);
        return 0;
    }
    printf("data size: %llu, node_size: %llu\n",sizeof(data),self->node_size);
    if(sizeof(data) >= self->node_size) {
        printf("data size is greater than capable linked list node data size\n");
    }
    memcpy_s(new_node->data,self->node_size, data, self->node_size);
    new_node->next = NULL;
    if (self->curr) {
        self->curr->next = new_node;
    } else {
        self->head = new_node;
    }
    self->curr = new_node;
    self->size++;
    return 1;
}

bool linkedlist_search(const linked_list* self, const void* value, i32 (*compare)(const void*, const void*)) {
    if (!self || !value || !compare) return 0;
    ll_node* current = self->head;
    while (current) {
        if (compare(current->data, value) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

bool linkedlist_delete_position(linked_list* self, u64 position) {
    if (!self || position >= self->size) return 0;
    ll_node* current = self->head;
    ll_node* previous = NULL;
    for (u64 i = 0; i < position; i++) {
        previous = current;
        current = current->next;
    }
    if (!previous) {
        self->head = current->next;
    } else {
        previous->next = current->next;
    }
    free(current->data);
    free(current);
    self->size--;
    return 1;
}

bool linkedlist_delete(linked_list* self, const void* value, i32 (*compare)(const void*, const void*)) {
    if (!self || !value || !compare) return 0;
    ll_node* current = self->head;
    ll_node* previous = NULL;
    while (current) {
        if (compare(current->data, value) == 0) {
            if (!previous) {
                self->head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current->data);
            free(current);
            self->size--;
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

void* linkedlist_get(const linked_list* self, u64 position) {
    if (!self || position >= self->size) return NULL;
    ll_node* current = self->head;
    for (u64 i = 0; i < position; i++) {
        current = current->next;
    }
    return current->data;
}

u64 linkedlist_size(const linked_list* self) {
    return self ? self->size : 0;
}

void linkedlist_destroy(linked_list* self, void (*destroy)(void*)) {
    if (!self) return;
    ll_node* current = self->head;
    ll_node* next;
    while (current) {
        printf("%p\n",(void*)current);
        next = current->next;
        if (destroy) {
            destroy(current->data);
        } else {
            free(current->data);
        }
        free(current);
        current = next;
    }
    free(self);
}
