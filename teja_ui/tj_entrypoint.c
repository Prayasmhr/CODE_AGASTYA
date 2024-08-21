#include"tj_entrypoint.h"
#include"stdio.h"
#include"stdlib.h"

tj_entrypoint tj_entrypoint_create(tj_appstate* app_state, tj_entrypoint_main_fn _application_main){
    return (tj_entrypoint) {
        .app_state = app_state,
        .application_main = _application_main,
        .ret_code = 0
    };
}

void tj_entrypoint_start(tj_entrypoint* entry_point){
    entry_point->ret_code = entry_point->application_main(entry_point->app_state);
}

void tj_entrypoint_destroy(tj_entrypoint* entry_point){
    printf("exiting with app return code: %d\n", entry_point->ret_code);
    free(entry_point);
}
