#ifndef _TJ_BUTTON_H
#define _TJ_BUTTON_H

#include"../heart_lib/lib.h"
#include"tj_renderer.h"
#include"tj_ui_entity.h"

typedef void (*btn_callback)(tj_appstate*,void*);

typedef struct _tj_button_T _begin_implement_uientt 
    btn_callback callback;
    void* callback_state;
    tj_color bg;
    tj_color fg;
    tj_image* image;
    tj_font* font;
    string text_buffer;
    bool is_hovering;
    u8 corner_radius;
_end_implement_uientt tj_button;

tj_button new_tj_button(btn_callback cb,tj_color bg,tj_color fg,tj_image* img,tj_font* font,string text_buf);
bool button_on_create(tj_button self);
bool button_on_update(tj_button self, tj_appstate* app_state);
bool button_on_render(tj_button self);
bool button_on_destroy(tj_button self);

#endif
