#ifndef _TJ_UI_ENTITY_H
#define _TJ_UI_ENTITY_H

#include"heart_lib/ht_defines.h"
#include"tj_appstate.h"
#include"tj_renderer.h"
#include"tj_asset.h"
#include"tj_vec2.h"
#include"stdlib.h"

typedef enum _tj_uientity_type{
    TJ_UIENTITY_BUTTON,
    TJ_UIENTITY_LABEL,
    TJ_UIENTIT_LAYOUT,
    TJ_UIENTITY_SCROLLBAR,
    TJ_UIENTITY_OPTIONMENU,
    TJ_UIENTITY_TABMENU,
} tj_uientity_type;

#define _begin_implement_uientt { tj_uientity base;
#define _end_implement_uientt }*
typedef struct tj_uientity tj_uientity;

typedef bool (*uientt_oncreate_fn)(tj_uientity*);
typedef bool (*uientt_onupdate_fn)(tj_uientity*,tj_appstate*);
typedef bool (*uientt_onrender_fn)(tj_uientity*);
typedef bool (*uientt_ondestroy_fn)(tj_uientity*);

typedef struct tj_uientity_ginfo {
    tj_vec2i position;
    tj_vec2i dimension;
    f32 rotation;
    f32 scale;
} tj_uientity_ginfo;

struct tj_uientity {
    tj_uientity_ginfo geometry_info;
    bool is_dirty;
    tj_uientity_type type;

    uientt_oncreate_fn on_create;
    uientt_onupdate_fn on_update;       
    uientt_onrender_fn on_render;
    uientt_ondestroy_fn on_destroy;
};

inline void tj_uientity_destroy(tj_uientity* uientt) {
    uientt->on_destroy(uientt);
    free(uientt);
}

#endif
