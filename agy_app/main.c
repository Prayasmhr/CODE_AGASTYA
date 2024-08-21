#include"agy_app.h"

#undef main

int main(int argc,const char* argv[]){
    tj_appstate app_state = (tj_appstate) {
        .inst = malloc(sizeof(_tj_appobject)),
        .running = 1,
    };

    tj_entrypoint entry_point = tj_entrypoint_create(&app_state, agy_app_main);

    tj_entrypoint_start(&entry_point);
    tj_entrypoint_destroy(&entry_point);

    return 0;
}
