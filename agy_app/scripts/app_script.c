#include"../module.h"

_EXPORT void change_color(tj_appstate* app_state,void* ctx_handle) {
    tj_label label = (tj_label)tj_uienv_get(app_state->inst->active_ui_env, "project_agy_label");
    label->fg = (tj_color) {.R = rand() % 255,.B = rand() % 255, .G = rand() % 255,.A = 255};
    label = NULL;
    return;
} 

_EXPORT void main_sector_render(tj_uisector* uisector) {

}
