#include"tj_uienv.h"
#include<stdlib.h>

uienv_btree uienv_tree;

typedef struct tj_uienv_layout_entry {
    string key;
    tj_uientity* entity;
    struct tj_uienv_layout_entry* next;
} tj_uienv_layout_entry;

struct tj_uienv_layout {
    tj_uienv_layout_entry** uientt_entries;
    u32* hashes;
};

static u16 uienv_hash(char *str,u16 capacity) {
    u32 hash = 5381;
    i32 c = 0;
    while ((c = *(str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % capacity;
}

tj_uienv tj_uienv_init(char* env_name,u16 capacity,tj_uientity** ui_entities,string* tags) {
    tj_uienv ui_env = (tj_uienv) {
        .capacity = capacity,
        .size = 0,
        .layout = malloc(sizeof(tj_uienv_layout))
    };
    ui_env.layout->uientt_entries = calloc(ui_env.capacity, sizeof(tj_uienv_layout_entry));
    ui_env.layout->hashes = malloc(sizeof(u32) * ui_env.capacity);
    for(u32 i = 0;i < capacity; i++) {
        tj_uienv_push(&ui_env,ui_entities[i],tags[i].data);
        string_free(&tags[i]);
    }
    ui_env.tag = new_string(env_name);
    return ui_env;
}

void tj_uienv_push(tj_uienv* ui_env,tj_uientity* uientt,char* tag) {
    u32 h_index = uienv_hash(tag,ui_env->capacity);
    if(ui_env->layout->uientt_entries[h_index] != NULL && strncmp(ui_env->layout->uientt_entries[h_index]->key.data,tag,strnlen(tag,32)) == 0) {
        return;
    }
    tj_uienv_layout_entry* iter_entry = malloc(sizeof(tj_uienv_layout_entry));
    iter_entry->entity = uientt;
    iter_entry->key = new_string(tag);
    iter_entry->next = ui_env->layout->uientt_entries[h_index];
    ui_env->layout->uientt_entries[h_index] = iter_entry;
    iter_entry = NULL;
    if(ui_env->layout->uientt_entries[h_index]->next == NULL) ui_env->layout->hashes[ui_env->size++] = h_index;
}

tj_uientity* tj_uienv_get(tj_uienv* ui_env,char* tag) {
    u32 index = uienv_hash(tag,ui_env->capacity);
    tj_uienv_layout_entry *entry = ui_env->layout->uientt_entries[index];
    while (entry && strncmp(entry->key.data, tag, entry->key.size) != 0) {
        entry = entry->next;
    }
    return entry->entity;
}

void tj_uienv_tick(tj_uienv* ui_env,tj_appstate* app_state) {
    tj_uienv_layout_entry* iter_entry;
    for(u32 i = 0;i < ui_env->size; i++) {
        iter_entry = ui_env->layout->uientt_entries[ui_env->layout->hashes[i]];
        while(iter_entry) {
            iter_entry->entity->on_update(iter_entry->entity,app_state);
            iter_entry = iter_entry->next;
        }
    }
    iter_entry = NULL;
}

void tj_uienv_render(tj_uienv* ui_env) {
    tj_uienv_layout_entry* iter_entry;
    for(u32 i = 0;i < ui_env->size; i++) {
        iter_entry = ui_env->layout->uientt_entries[ui_env->layout->hashes[i]];
        while(iter_entry) {
            iter_entry->entity->on_render(iter_entry->entity);
            iter_entry = iter_entry->next;
        }
    }
    iter_entry = NULL;
}

void tj_uienv_free(tj_uienv* ui_env) {
    tj_uienv* env = (tj_uienv*)ui_env;
	for (u32 i = 0; i < env->capacity; i++) {
        tj_uienv_layout_entry* entry = env->layout->uientt_entries[i];
        while (entry) {
            tj_uienv_layout_entry* temp = entry;
            entry = entry->next;
            string_free(&temp->key);
            free(temp);
        }
    }
    free(env->layout->uientt_entries);
    free(env->layout->hashes);
    free(env->layout);
    free(env);
}

i32 tj_uienv_compare(const tj_uienv* lhs,const tj_uienv* rhs) {
    if(!lhs) return 0;
    return string_compare(lhs->tag, rhs->tag);
}

struct uienv_tree_node {
  tj_uienv data;
  uienv_tree_node *left;
  uienv_tree_node *right;
};
static uienv_tree_node *uienv_create_tree_node(tj_uienv data) {
  uienv_tree_node *out = malloc(sizeof(uienv_tree_node));
  if (!out)
    return ((void *)0);
  out->data = data;
  out->left = ((void *)0);
  out->right = ((void *)0);
  return out;
}
uienv_btree uienv_btree_init(uienv_compare_fn _compare,
                             uienv_destroy_data_fn _destroy_data) {
  uienv_btree self;
  self.data_size = sizeof(tj_uienv);
  self.max = ((void *)0);
  self.min = ((void *)0);
  self.root = ((void *)0);
  self.compare = _compare;
  self.destroy_data = _destroy_data;
  self.size = 0;
  return self;
}
_Bool uienv_btree_insert_node(uienv_btree *self, uienv_tree_node **node,
                              tj_uienv data) {
  if (*node == ((void *)0)) {
    *node = uienv_create_tree_node(data);
    return 1;
  }
  int cmp = self->compare(&data, &(*node)->data);
  if (cmp == 0)
    return 0;
  if (cmp < 0)
    return uienv_btree_insert_node(self, &(*node)->left, data);
  else
    return uienv_btree_insert_node(self, &(*node)->right, data);
}
_Bool uienv_btree_insert(uienv_btree *self, tj_uienv data) {
  if (!self)
    return 0;
  if (!self->root) {
    self->root = uienv_create_tree_node(data);
    self->max = self->root;
    self->min = self->root;
    self->size++;
    return 1;
  }
  if (uienv_btree_insert_node(self, &self->root, data)) {
    self->size++;
    return 1;
  }
  return 0;
}
static uienv_tree_node *uienv_btree_remove_node(uienv_btree *self,
                                                uienv_tree_node *node,
                                                tj_uienv data, _Bool *removed) {
  if (!node)
    return ((void *)0);
  int cmp = self->compare(&data, &node->data);
  if (cmp < 0) {
    node->left = uienv_btree_remove_node(self, node->left, data, removed);
  } else if (cmp > 0) {
    node->right = uienv_btree_remove_node(self, node->right, data, removed);
  } else {
    *removed = 1;
    _Bool is_left_null = node->left == ((void *)0);
    _Bool is_right_null = node->right == ((void *)0);
    if (is_left_null && is_right_null) {
      self->destroy_data(&node->data);
      free(node);
      return ((void *)0);
    } else if (!is_left_null || !is_right_null) {
      uienv_tree_node *temp = is_left_null ? node->right : node->left;
      self->destroy_data(&node->data);
      free(node);
      return temp;
    }
    uienv_tree_node *min_node = uienv_btree_find_min(node->right);
    node->data = min_node->data;
    node->right =
        uienv_btree_remove_node(self, node->right, min_node->data, removed);
  }
  return node;
}
_Bool uienv_btree_remove(uienv_btree *self, tj_uienv data) {
  if (!self || !self->root)
    return 0;
  _Bool removed = 0;
  self->root = uienv_btree_remove_node(self, self->root, data, &removed);
  if (removed) {
    self->size--;
    if (self->compare(&data, &self->min->data) == 0) {
      self->min = uienv_btree_find_min(self->root);
    }
    if (self->compare(&data, &self->max->data) == 0) {
      self->max = uienv_btree_find_max(self->root);
    }
  }
  return removed;
}
static uienv_tree_node *uienv_btree_find_min(uienv_tree_node *node) {
  while (node && node->left)
    node = node->left;
  return node;
}
static uienv_tree_node *uienv_btree_find_max(uienv_tree_node *node) {
  while (node && node->right)
    node = node->right;
  return node;
}
tj_uienv *uienv_btree_search(const uienv_btree *self, tj_uienv data) {
  if (!self || !self->root)
    return ((void *)0);
  uienv_tree_node *current = self->root;
  while (current) {
    int cmp = self->compare(&data, &current->data);
    if (cmp == 0)
      return &current->data;
    current = (cmp < 0) ? current->left : current->right;
  }
  return ((void *)0);
}
static void uienv_btree_free_node(uienv_btree *self, uienv_tree_node *node) {
  if (!node)
    return;
  uienv_btree_free_node(self, node->left);
  uienv_btree_free_node(self, node->right);
  self->destroy_data(&node->data);
  free(node);
}
void uienv_btree_free(uienv_btree *self) {
  if (!self)
    return;
  uienv_btree_free_node(self, self->root);
  free(self);
}
u64 uienv_btree_size(const uienv_btree *self) { return self ? self->size : 0; }
