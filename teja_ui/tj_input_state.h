#ifndef _TJ_INPUTSTATE_H
#define _TJ_INPUTSTATE_H

#include"tj_vec2.h"
#include"tj_appstate.h"
#include"SDL2/SDL.h"

#define KEY_COUNT 255

typedef struct {
	bool pressed;
	u8 times_pressed;
} key_state;

typedef enum {
	NONE,
	LEFT,
	MIDDLE,
	RIGHT,
} mouse_button;

void tj_inputstate_init(void);
tj_vec2i tj_inputstate_get_mpos(void);
tj_vec2i tj_inputstate_get_mrel(void);
void tj_inputstate_process_kpress(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_process_krelease(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_process_mmove(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_process_mscroll(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_process_mbpress(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_process_mbrelease(SDL_Event e, tj_appstate* app_state);
void tj_inputstate_flush(void);
bool tj_inputstate_is_mb_pressed(mouse_button button,bool repeat);
bool tj_inputstate_is_key_pressed(u8 key,bool repeat);
tj_vec2i tj_inputstate_get_mscroll(void);

#endif
