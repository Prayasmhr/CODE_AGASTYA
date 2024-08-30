#ifndef _TJ_UISECTOR_H
#define _TJ_UISECTOR_H

#include"heart_lib/ht_defines.h"
#include"heart_lib/ht_string.h"

typedef struct tj_uisector tj_uisector;
typedef void (*tj_uisector_render)(tj_uisector*);

struct tj_uisector {
    i32 x;
    i32 y;
    u32 w;
    u32 h;
    tj_uisector_render render;
    string* uientt_tags;
};

tj_uisector tj_uisector_create(i32 x,i32 y,u32 w,u32 h,tj_uisector_render _render);
void tj_uisector_free(tj_uisector* self);

#endif
