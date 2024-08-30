#include"tj_button.h"
#include"tj_input_state.h"

tj_button new_tj_button(btn_callback cb,tj_color bg,tj_color fg,tj_image* img,tj_font* font,string text_buf){
	tj_button self = malloc(sizeof(struct _tj_button_T));
	self->bg = bg;
	self->callback = cb;
	self->callback_state = NULL;
	self->font = font;
	self->image = img;
	self->is_hovering = 0;
	self->fg = fg;
	self->corner_radius = 0;
	self->text_buffer = text_buf;

    self->base.is_dirty = 0;
    self->base.type = TJ_UIENTITY_BUTTON;    
    self->base.on_create = (uientt_oncreate_fn)button_on_create;
    self->base.on_update = (uientt_onupdate_fn)button_on_update;
    self->base.on_render = (uientt_onrender_fn)button_on_render;
    self->base.on_destroy = (uientt_ondestroy_fn)button_on_destroy;

	return self;
}

static bool is_mouse_hovering(tj_uientity_ginfo geometry_info) {
	tj_vec2i mouse_pos = tj_inputstate_get_mpos();
	return (mouse_pos.x < (geometry_info.position.x + geometry_info.dimension.x) && mouse_pos.x > geometry_info.position.x) &&
		(mouse_pos.y < (geometry_info.position.y + geometry_info.dimension.y) && mouse_pos.y > geometry_info.position.y);
}

bool button_on_create(tj_button self) {
	self->is_hovering = 0;
	if(self->image) {
		self->base.geometry_info.dimension = render_engine.get_image_size(self->image);
	}
	return 1;
}

bool button_on_update(tj_button self, tj_appstate* app_state) {
	if (is_mouse_hovering(self->base.geometry_info)) {
		self->is_hovering = 1;
	}else {
		self->is_hovering = 0;
	}
	if (self->is_hovering && tj_inputstate_is_mb_pressed(LEFT,0)) {
		self->callback(app_state,self->callback_state);
	}

	return 1;
}

bool button_on_render(tj_button self) {
	render_engine.render_rect((tj_rectangle){
		self->base.geometry_info.position.x, self->base.geometry_info.position.y,
		self->base.geometry_info.dimension.x, self->base.geometry_info.dimension.y,
		self->corner_radius}, 
		self->bg, 0
	);
	if(self->image) {
		render_engine.render_image(self->image, self->base.geometry_info.position);
	}
	if(self->font) {
		tj_vec2i text_size = render_engine.get_font_size(self->font, self->text_buffer.data);
		tj_vec2i text_offset = (tj_vec2i){
			.x = (self->base.geometry_info.position.x - text_size.x / 2 + self->base.geometry_info.dimension.x / 2),
			.y = (self->base.geometry_info.position.y - text_size.y / 2 + self->base.geometry_info.dimension.y / 2)
		};

		render_engine.render_text(
			self->font, self->fg, self->bg,text_offset,
			self->text_buffer.size + 1, self->text_buffer.data
		);
	}
	return 1;
}

bool button_on_destroy(tj_button self) {
	string_free(&self->text_buffer);
	self->font = NULL;
	self->image = NULL;	
	free(self);
	return 1;
}
