#include"photon8_core.h"
#include"photon8_renderer_export.h"

#include"../teja_ui/tj_renderer.h"
#include"../teja_ui/tj_asset.h"
#include"../teja_ui/tj_event_handler.h"
#include"memory.h"
#include"stdarg.h"
#include<stdio.h>

struct tj_render_backend {
    SDL_Window* window;
    photon8_core pn8_core;
    struct {
        u32 window_width;
        u32 window_height;
        u32 logical_width;
        u32 logical_height;
        string title;
   } config;
};

typedef tj_render_backend pn8_render_backend;
tj_render_engine render_engine;

PHOTON8_RENDERER_API bool pn8_renderer_init(tj_render_config render_create_specs){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Vulkan_LoadLibrary(NULL);

    render_engine.config.window_width = render_create_specs.window_width;
    render_engine.config.window_height = render_create_specs.window_height;
    render_engine.config.logical_width = render_create_specs.logical_width;
    render_engine.config.logical_height = render_create_specs.logical_height;
    render_engine.config.title = render_create_specs.title;

    render_engine.render_backend = malloc(sizeof(pn8_render_backend));

    render_engine.render_backend->window = SDL_CreateWindow(
        render_create_specs.title.data,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    pn8_core_init(&render_engine.render_backend->pn8_core,render_engine.render_backend->window);
}