#ifndef _TJ_EVENTHANDLER_H
#define _TJ_EVENTHANDLER_H

#include"../vendor/SDL2/SDL.h"
#include"../heart_lib/lib.h"
#include"tj_appstate.h"

#define MAX_EVENT_COUNT 32512

typedef void (*callback_fn)(SDL_Event e,tj_appstate* e_state);

typedef struct {
    callback_fn callback;
    bool valid;
} tj_eventcallback;

static struct {
    SDL_Event event;
    tj_eventcallback callback_response[MAX_EVENT_COUNT];
} event_handler;


void tj_event_handler_init(void);
void tj_event_handler_pump(tj_appstate* app_state);
void tj_event_handler_register(SDL_EventType e_type,callback_fn callback); 
void tj_event_handler_shutdown(void);

#endif
