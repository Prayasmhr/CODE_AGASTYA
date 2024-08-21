#ifndef _TJ_UIENV_H
#define _TJ_UIENV_H

#include"../teja_ui/tj_ui_entity.h"

typedef struct tj_uienv_layout tj_uienv_layout;

typedef struct tj_uienv {
  string tag;
  tj_uienv_layout* layout;
  u16 size;
  u16 capacity;
} tj_uienv;

_BINARY_TREE_PROTOTYPE(tj_uienv, uienv)

extern uienv_btree uienv_tree;

tj_uienv tj_uienv_init(char* env_name,u16 capacity,tj_uientity** ui_entities,string* tags);
void tj_uienv_push(tj_uienv* ui_env,tj_uientity* uientt,char* tag);
tj_uientity* tj_uienv_get(tj_uienv* ui_env,char* tag);
void tj_uienv_tick(tj_uienv* ui_env,tj_appstate* app_state);
void tj_uienv_render(tj_uienv* ui_env);
void tj_uienv_free(tj_uienv* ui_env);
i32 tj_uienv_compare(const tj_uienv* lhs,const tj_uienv* rhs);
static u16 uienv_hash(char* str,u16 capacity);

inline static void tj_uienv_tree_init(void) { uienv_tree = uienv_btree_init(tj_uienv_compare,tj_uienv_free);}
inline static void tj_uienv_tree_insert(tj_uienv ui_env) { uienv_btree_insert(&uienv_tree,ui_env);}
inline static tj_uienv* tj_uienv_tree_get(char* tag) { return uienv_btree_search(&uienv_tree,(tj_uienv) {.tag = new_string(tag)}); }
inline static void tj_uienv_tree_free(void) { uienv_btree_free(&uienv_tree); }

#endif
