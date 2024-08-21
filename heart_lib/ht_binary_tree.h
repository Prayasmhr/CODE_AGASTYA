#ifndef _HT_BINARY_TREE_H
#define _HT_BINARY_TREE_H

#include"ht_defines.h"

#define _BINARY_TREE_PROTOTYPE(type, name) \
    typedef struct name##_tree_node name##_tree_node;\
    typedef i32 (*name##_compare_fn)(const type* lhs, const type* rhs);\
    typedef void (*name##_destroy_data_fn)(type* data);\
    \
    typedef struct name##_btree {\
        name##_tree_node* root;\
        name##_tree_node* min;\
        name##_tree_node* max;\
        u64 data_size;\
        name##_compare_fn compare;\
        name##_destroy_data_fn destroy_data;\
        u64 size;\
    } name##_btree;\
    \
    name##_btree name##_btree_init(name##_compare_fn compare, name##_destroy_data_fn destroy_data);\
    bool name##_btree_insert_node(name##_btree* self,name##_tree_node** node, type data);\
    bool name##_btree_insert(name##_btree* self, type data);\
    bool name##_btree_remove(name##_btree* self, type data);\
    static name##_tree_node* name##_btree_remove_node(name##_btree* self, name##_tree_node* node, type data, bool* removed);\
    static name##_tree_node* name##_btree_find_min(name##_tree_node* node);\
    static name##_tree_node* name##_btree_find_max(name##_tree_node* node);\
    static void name##_btree_free_node(name##_btree* self, name##_tree_node* node);\
    type* name##_btree_search(const name##_btree* self, type data);\
    void name##_btree_free(name##_btree* self);\
    u64 name##_btree_size(const name##_btree* self);\
    \

#define _BINARY_TREE_IMPLEMENT(type, name) \
    struct name##_tree_node {\
        type data;\
        name##_tree_node* left;\
        name##_tree_node* right;\
    };\
    \
    static name##_tree_node* name##_create_tree_node(type data) {\
        name##_tree_node* out = malloc(sizeof(name##_tree_node));\
        if (!out) return NULL;\
        out->data = data;\
        out->left = NULL;\
        out->right = NULL;\
        return out;\
    }\
    \
    name##_btree name##_btree_init(name##_compare_fn _compare, name##_destroy_data_fn _destroy_data) {\
        name##_btree self;\
        self.data_size = sizeof(type);\
        self.max = NULL;\
        self.min = NULL;\
        self.root = NULL;\
        self.compare = _compare;\
        self.destroy_data = _destroy_data;\
        self.size = 0;\
        return self;\
    }\
    \
    bool name##_btree_insert_node(name##_btree* self,name##_tree_node** node, type data) {\
        if(*node == NULL) {\
            *node = name##_create_tree_node(data);\
            return 1;\
        }\
        int cmp = self->compare(&data, &(*node)->data);\
        if(cmp == 0) return 0;\
    \
        if(cmp < 0) return name##_btree_insert_node(self,&(*node)->left,data);\
        else return name##_btree_insert_node(self,&(*node)->right,data);\
    }\
    \
    bool name##_btree_insert(name##_btree *self, type data){\
        if (!self)\
            return 0;\
        if (!self->root){\
            self->root = name##_create_tree_node(data);\
            self->max = self->root;\
            self->min = self->root;\
            self->size++;\
            return 1;\
        }\
        if(name##_btree_insert_node(self,&self->root,data)) {\
            self->size++;\
            return 1;\
        }\
        return 0;\
    }\
    \
    static name##_tree_node* name##_btree_remove_node(name##_btree* self, name##_tree_node* node, type data, bool* removed) {\
        if (!node) return NULL;\
        \
        int cmp = self->compare(&data, &node->data);\
        if (cmp < 0) {\
            node->left = name##_btree_remove_node(self, node->left, data, removed);\
        } else if (cmp > 0) {\
            node->right = name##_btree_remove_node(self, node->right, data, removed);\
        } else {\
            *removed = 1;\
            bool is_left_null = node->left == NULL;\
            bool is_right_null = node->right == NULL;\
            if (is_left_null && is_right_null) {\
                self->destroy_data(&node->data);\
                free(node);\
                return NULL;\
            } else if (!is_left_null || !is_right_null) {\
                name##_tree_node* temp = is_left_null ? node->right : node->left;\
                self->destroy_data(&node->data);\
                free(node);\
                return temp;\
            }\
            \
            name##_tree_node* min_node = name##_btree_find_min(node->right);\
            node->data = min_node->data;\
            node->right = name##_btree_remove_node(self, node->right, min_node->data, removed);\
        }\
        return node;\
    }\
    \
    bool name##_btree_remove(name##_btree* self, type data) {\
        if (!self || !self->root) return 0;\
        \
        bool removed = 0;\
        self->root = name##_btree_remove_node(self, self->root, data, &removed);\
        \
        if (removed) {\
            self->size--;\
            if (self->compare(&data, &self->min->data) == 0) {\
                self->min = name##_btree_find_min(self->root);\
            }\
            if (self->compare(&data, &self->max->data) == 0) {\
                self->max = name##_btree_find_max(self->root);\
            }\
        }\
        \
        return removed;\
    }\
    \
    static name##_tree_node* name##_btree_find_min(name##_tree_node* node) {\
        while (node && node->left) node = node->left;\
        return node;\
    }\
    static name##_tree_node* name##_btree_find_max(name##_tree_node* node) {\
        while (node && node->right) node = node->right;\
        return node;\
    }\
    \
    type* name##_btree_search(const name##_btree* self, type data) {\
        if (!self || !self->root) return NULL;\
        \
        name##_tree_node* current = self->root;\
        while (current) {\
            int cmp = self->compare(&data, &current->data);\
            if (cmp == 0) return &current->data;\
            current = (cmp < 0) ? current->left : current->right;\
        }\
        \
        return NULL; \
    }\
    \
    static void name##_btree_free_node(name##_btree* self, name##_tree_node* node) {\
        if (!node) return;\
        \
        name##_btree_free_node(self, node->left);\
        name##_btree_free_node(self, node->right);\
        \
        self->destroy_data(&node->data);\
        free(node);\
    }\
    \
    void name##_btree_free(name##_btree* self) {\
        if (!self) return;\
        name##_btree_free_node(self, self->root);\
        free(self);\
    }\
    \
    u64 name##_btree_size(const name##_btree* self) {\
        return self ? self->size : 0;\
    }\
    \

#endif
