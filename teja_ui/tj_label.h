#ifndef _TJ_LABEL_H
#define _TJ_LABEL_H

#include"tj_ui_entity.h"

typedef struct _tj_label_T _begin_implement_uientt 
    tj_image* image;
    tj_font* font;
    tj_color fg;
    tj_color bg;
    string text_buffer;
_end_implement_uientt tj_label;

tj_label new_tj_label(string text_buf,tj_color fg,tj_color bg,tj_font* font,tj_image* image);
bool label_on_create(tj_label self);
bool label_on_update(tj_label self, tj_appstate* app_state);
bool label_on_render(tj_label self);
bool label_on_destroy(tj_label self);

#endif
