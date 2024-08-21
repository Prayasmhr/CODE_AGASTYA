#include"tj_uienv.h"
#include<stdlib.h>

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

_BINARY_TREE_IMPLEMENT(tj_uienv, uienv)
