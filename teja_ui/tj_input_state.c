#include"tj_input_state.h"

typedef struct {
	struct {
		tj_vec2i position;
		tj_vec2i rel;
		tj_vec2i scroll;
		mouse_button button;
		bool pressed;
		bool released;
	} mouse_state;

	struct {
		key_state keys[KEY_COUNT];
	} keyboard_state;
} TJ_InputState;

TJ_InputState input_state;
TJ_InputState prev_input_state;

static void store_prev_keyboard(SDL_Event e) {
	prev_input_state.keyboard_state.keys[e.key.keysym.sym].pressed =
		input_state.keyboard_state.keys[e.key.keysym.sym].pressed;
}

static void store_prev_mouse_buttons(void) {
	prev_input_state.mouse_state.button = input_state.mouse_state.button;
	prev_input_state.mouse_state.pressed = input_state.mouse_state.pressed;
	prev_input_state.mouse_state.released = input_state.mouse_state.released;
}

void tj_inputstate_init(void) {
	input_state.mouse_state.position = (tj_vec2i){ .x = 0,.y = 0 };
	input_state.mouse_state.rel = (tj_vec2i){ .x = 0,.y = 0 };
	input_state.mouse_state.scroll = (tj_vec2i){ .x = 0,.y = 0 };

	input_state.mouse_state.button = NONE;
	input_state.mouse_state.pressed = 0;
	input_state.mouse_state.released = 0;

	for (int i = 0; i < KEY_COUNT; i++) {
		input_state.keyboard_state.keys[i].pressed = 0;
		input_state.keyboard_state.keys[i].times_pressed = 0;
	}
}

tj_vec2i tj_inputstate_get_mpos(void) {
	return input_state.mouse_state.position;
}

tj_vec2i tj_inputstate_get_mrel(void) {
	return input_state.mouse_state.rel;
}

bool tj_inputstate_is_mb_pressed(mouse_button button, bool repeat) {
	if (input_state.mouse_state.pressed && !prev_input_state.mouse_state.pressed) {
        return 1;
    }
    if (input_state.mouse_state.released && !prev_input_state.mouse_state.released) {
        input_state.mouse_state.released = 0; 
        return !repeat; 
    }
    return 0;
}

bool tj_inputstate_is_key_pressed(u8 key,bool repeat){
	return input_state.keyboard_state.keys[key].pressed && (repeat ? 1 : prev_input_state.keyboard_state.keys[key].pressed == 1);
}

tj_vec2i tj_inputstate_get_mscroll(void) {
	return input_state.mouse_state.scroll;
}

void tj_inputstate_process_kpress(SDL_Event e, tj_appstate* app_state) {
	store_prev_keyboard(e);
	input_state.keyboard_state.keys[e.key.keysym.sym].times_pressed += input_state.keyboard_state.keys[e.key.keysym.sym].pressed;
	input_state.keyboard_state.keys[e.key.keysym.sym].pressed = 1;
}

void tj_inputstate_process_krelease(SDL_Event e, tj_appstate* app_state) {
	store_prev_keyboard(e);
	input_state.keyboard_state.keys[e.key.keysym.sym].times_pressed = 0;
	input_state.keyboard_state.keys[e.key.keysym.sym].pressed = 0;
}

void tj_inputstate_process_mmove(SDL_Event e, tj_appstate* app_state) {
	prev_input_state.mouse_state.rel = input_state.mouse_state.rel;
	prev_input_state.mouse_state.position = input_state.mouse_state.position;
	input_state.mouse_state.rel = (tj_vec2i){ .x = e.motion.xrel, .y = e.motion.yrel };
	input_state.mouse_state.position = (tj_vec2i){ .x = e.motion.x, .y = e.motion.y };
}

void tj_inputstate_process_mscroll(SDL_Event e, tj_appstate* app_state) {
	prev_input_state.mouse_state.scroll = input_state.mouse_state.scroll;
	input_state.mouse_state.scroll.x = e.wheel.x;
	input_state.mouse_state.scroll.y = e.wheel.y;
}

void tj_inputstate_process_mbpress(SDL_Event e, tj_appstate* app_state) {
	store_prev_mouse_buttons();
	input_state.mouse_state.button = e.button.button;
	input_state.mouse_state.pressed = 1;
	input_state.mouse_state.released = 0;
}

void tj_inputstate_flush(void){
	prev_input_state.mouse_state.button = NONE;
	prev_input_state.mouse_state.pressed = 0;
}

void tj_inputstate_process_mbrelease(SDL_Event e, tj_appstate* app_state) {
	store_prev_mouse_buttons();
	input_state.mouse_state.button = NONE;
	input_state.mouse_state.pressed = 0;
	input_state.mouse_state.released = 1;
}

