#include"tj_event_handler.h"

void tj_event_handler_init(void){
    for(int i = 0;i < MAX_EVENT_COUNT; i++){
        event_handler.callback_response[i].callback = NULL;
        event_handler.callback_response[i].valid = 0;
    }
    printf("event handler initialized\n");
}

void tj_event_handler_pump(tj_appstate* app_state){
    while(SDL_PollEvent(&event_handler.event)){
        if(event_handler.callback_response[event_handler.event.type].valid){
            callback_fn callback = event_handler.callback_response[event_handler.event.type].callback;
            callback(event_handler.event,app_state);
        }
    }
}

void tj_event_handler_register(SDL_EventType e_type,callback_fn callback){
    event_handler.callback_response[e_type].callback = callback;
    event_handler.callback_response[e_type].valid = 1;
    printf("registering a function of type :%d\n",e_type);
}

void tj_event_handler_shutdown(void){
    for(int i = 0;i < MAX_EVENT_COUNT; i++){
        memset(&event_handler.callback_response[i],0,sizeof(tj_eventcallback));
    }
}
