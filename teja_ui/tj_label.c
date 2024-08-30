#include"tj_label.h"
#include"stdlib.h"

tj_label new_tj_label(string text_buf,tj_color fg,tj_color bg,tj_font* font,tj_image* image) {
    tj_label self = malloc(sizeof(struct _tj_label_T));
    self->fg = fg;
    self->bg = bg;
    self->font = font;
    self->image = image;
    self->text_buffer = text_buf;

    self->base.is_dirty = 0;
    self->base.type = TJ_UIENTITY_LABEL;    
    self->base.on_create = (uientt_oncreate_fn)label_on_create;
    self->base.on_update = (uientt_onupdate_fn)label_on_update;
    self->base.on_render = (uientt_onrender_fn)label_on_render;
    self->base.on_destroy = (uientt_ondestroy_fn)label_on_destroy;

    self->base.on_create((tj_uientity*)self);

    return self;
}

bool label_on_create(tj_label self) {
    if(self->image) {
        self->base.geometry_info.dimension = render_engine.get_image_size(self->image);
    }
    return 1;
}

bool label_on_update(tj_label self, tj_appstate* app_state) {
    return 1;
}

bool label_on_render(tj_label self) {
    tj_vec2i text_offset = self->base.geometry_info.position;
    if(self->image){
        render_engine.render_image(self->image, self->base.geometry_info.position);
        if(!self->font){
            return 1;
        }
        tj_vec2i text_size = render_engine.get_font_size(self->font, self->text_buffer.data);
        text_offset = (tj_vec2i){
			.x = (self->base.geometry_info.position.x - text_size.x / 2 + self->base.geometry_info.dimension.x / 2),
			.y = (self->base.geometry_info.position.y - text_size.y / 2 + self->base.geometry_info.dimension.y / 2)
		};
    } 
    if(self->font){
        render_engine.render_text(self->font, self->fg, self->bg, text_offset, self->text_buffer.size + 1,self->text_buffer.data);
    }

    return 1;
}

bool label_on_destroy(tj_label self) {
    self->font = NULL;
    self->image = NULL;

    string_free(&self->text_buffer);
    free(self);

    return 1;
}
