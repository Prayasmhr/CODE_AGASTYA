#include"agy_app.h"
#include"../tjlang/tjlang.h"
#include"stdio.h"

static void app_on_quit(SDL_Event e, tj_appstate* app_state){
    app_state->running = 0;
}

static void app_on_window_resize(SDL_Event e,tj_appstate* app_state) {
    //TODO: handle window resize 
}

bool agy_app_init(tj_appstate* self) {
    printf("initalizing app\n");
    srand(time(NULL));

    uienv_tree = uienv_btree_init(tj_uienv_compare,tj_uienv_free);

    tj_render_config render_config = (tj_render_config) {
        .window_width = 1000,
        .window_height = 800,
        .title = new_string("Code: Agastya"),
        .logical_width = 1920,
        .logical_height = 1080,
        .backend_type = RAY_RENDERER
    };

    tj_render_load_backend(render_config.backend_type);

    render_engine.init(render_config);
    tjlang_interpret("agy_app","agy_app/scripts/main_mode.tjscript");

    self->inst->message = new_nstring("Hello", 6);
    self->inst->active_ui_env = tj_uienv_tree_get("welcome_env");
    asset_btree_insert(&asset_tree,(tj_asset){.asset.font_handle = render_engine.create_font("assets/fonts/JetBrainsMonoNL-Bold.ttf",64),.type = FONT,.asset_tag = new_string("test_font32")});
    self->inst->font = asset_btree_search_by_tag("test_font32")->asset.font_handle;
    if(self->inst->active_ui_env == NULL) {
        printf("got it \n");
    }

    tj_event_handler_register(SDL_QUIT, app_on_quit);
    tj_event_handler_register(SDL_WINDOWEVENT, app_on_window_resize);
    tj_event_handler_register(SDL_KEYDOWN, tj_inputstate_process_kpress);
    tj_event_handler_register(SDL_KEYUP, tj_inputstate_process_krelease);
    tj_event_handler_register(SDL_MOUSEBUTTONDOWN, tj_inputstate_process_mbpress);
    tj_event_handler_register(SDL_MOUSEBUTTONUP, tj_inputstate_process_mbrelease);
    tj_event_handler_register(SDL_MOUSEMOTION, tj_inputstate_process_mmove);

    return 1;
}

bool agy_app_shutdown(tj_appstate* self) {

    string_free(&self->inst->message);
    tj_uienv_tree_free();
    render_engine.shutdown();
    return 1;
}

i32 agy_app_main(tj_appstate *app_state)
{
    tj_event_handler_init();
    agy_app_init(app_state);

    while (app_state->running) { 
        tj_event_handler_pump(app_state);

        render_engine.clear((tj_color) { .R = 75, .G = 0,. B = 0,.A = 0});
        
        tj_uienv_tick(app_state->inst->active_ui_env,app_state);
        tj_uienv_render(app_state->inst->active_ui_env);
        render_engine.render_circle( (tj_vec2i) {.x = 650,.y = 650}, 64, (tj_color) {.R = 255,.G = 255,.B = 255, .A= 255});
        render_engine.render_text(app_state->inst->font,(tj_color){.R = 255,.G = 255,.B = 255,. A = 255},(tj_color){.R = 255,.G = 0,.B = 0,.A = 0},(tj_vec2i){.x = 0,.y = 0},16,"Hello World");
        render_engine.flip();
    }

    agy_app_shutdown(app_state);
    tj_event_handler_shutdown();

    return 0;
}
