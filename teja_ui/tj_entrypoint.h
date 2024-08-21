#ifndef _TJ_ENTRYPOINT_H
#define _TJ_ENTRYPOINT_H

#include"tj_appstate.h"
#include"../heart_lib/ht_defines.h"

typedef i32 (*tj_entrypoint_main_fn)(tj_appstate*);

typedef struct tj_entrypoint {
    tj_appstate* app_state;
    tj_entrypoint_main_fn application_main;
    i32 ret_code;
} tj_entrypoint;

tj_entrypoint tj_entrypoint_create(tj_appstate* app_state, tj_entrypoint_main_fn _application_main);
void tj_entrypoint_start(tj_entrypoint* entry_point);
void tj_entrypoint_destroy(tj_entrypoint* entry_point);

#endif
