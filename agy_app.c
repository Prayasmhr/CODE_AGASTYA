#include"agy_app/module.h"

void app_hello (tj_appstate* app_state, gcptr ctx_handle) {
    printf("%s\n",app_state->inst->message.data);
    return;
}

