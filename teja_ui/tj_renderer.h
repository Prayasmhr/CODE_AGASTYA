#ifndef _TJ_RENDERER_H
#define _TJ_RENDERER_H

#include"../heart_lib/lib.h"
#include"tj_asset.h"
#include"tj_vec2.h"
#include"tj_render_types.h"

/*
    Render Backend 
    Here the ray renderer is a wrapper backend to sdl2 library.
    Photon8 renderer is a renderer written using vulkan graphics API.
    The render backend are separate and compiled into shared libraries or dynamic libraries and loaded (at runtime).
    The config specifies the backend to be loaded.
*/

typedef enum tj_render_backend_type {
    RAY_RENDERER, //sdl2 wrapper
    PHOTON8_RENDERER, //made with vulkan
} tj_render_backend_type;

typedef struct tj_renderconfig{
    u32 window_width;
    u32 window_height;
    u32 logical_width;
    u32 logical_height;
    string title;
    tj_render_backend_type backend_type;
} tj_render_config;

typedef struct tj_render_engine tj_render_engine;
typedef struct tj_render_backend tj_render_backend; //backend is not defined in the whole tj_renderer.h , it is defined in different backends. 
/*
    Render engine struct has the backend layer.
    It is one of the core componenents of the TejaUI library. 
    Its mainly responsible for rendering and has various backend dependent code connection through function pointers
*/


typedef bool            (*tj_render_engine_init_fn)             (tj_render_config);
typedef void            (*tj_render_engine_shutdown_fn)         (void);
typedef tj_image*       (*tj_render_engine_create_image_fn)     (char*);
typedef tj_font*        (*tj_render_engine_create_font_fn)      (char* ,u16 );
typedef void            (*tj_render_engine_destroy_font_fn)     (tj_font* );
typedef void            (*tj_render_engine_destroy_image_fn)    (tj_image*);
typedef void            (*tj_render_engine_clear_fn)            (tj_color );
typedef void            (*tj_render_engine_render_image_fn)     (tj_image*,tj_vec2i ); 
typedef void            (*tj_render_engine_render_text_fn)      (tj_font*,tj_color,tj_color,tj_vec2i,u32,char*,...);
typedef void            (*tj_render_engine_render_rect_fn)      (tj_rectangle,tj_color,u8);
typedef void            (*tj_render_engine_render_circle_fn)    (tj_vec2i, u8, tj_color);
typedef void            (*tj_render_engine_render_line_fn)      (tj_vec2i, tj_vec2i, tj_color);
typedef void            (*tj_render_engine_flip_fn)             (void);
typedef tj_vec2i        (*tj_render_engine_get_image_size_fn)   (tj_image*);
typedef tj_vec2i        (*tj_render_engine_get_font_size_fn)    (tj_font* font,const char* buffer);
typedef void            (*tj_render_engine_set_image_size_fn)   (tj_image*,tj_vec2i);

struct tj_render_engine {
    tj_render_backend* render_backend;
    tj_render_config config;

    tj_render_engine_init_fn init;
    tj_render_engine_shutdown_fn shutdown; 

    tj_render_engine_create_image_fn create_image;
    tj_render_engine_destroy_image_fn destroy_image;
    tj_render_engine_create_font_fn create_font;
    tj_render_engine_destroy_font_fn destroy_font;

    tj_render_engine_clear_fn clear;
    tj_render_engine_render_image_fn render_image;
    tj_render_engine_render_text_fn render_text;
    tj_render_engine_render_rect_fn render_rect;
    tj_render_engine_render_circle_fn render_circle;
    tj_render_engine_render_line_fn render_line;
    tj_render_engine_flip_fn flip;

    tj_render_engine_get_font_size_fn get_font_size;
    tj_render_engine_get_image_size_fn get_image_size;
    tj_render_engine_set_image_size_fn set_image_size;
};

/*
    tj_render_engine.init -> initializes the render engine with the config and also the backend. 
    tj_render_engine.shutdown -> shutdowns the render engine and the backend. 
    tj_render_engine.flip -> flips the renderer | shows the new pixels sended for rendering.
    tj_render_engine.clear -> clears the renderer with the color | fills the whole renderer extent with a color.
*/

extern tj_render_engine render_engine; // render engine is global to dont have the render handle complexity

void tj_render_load_backend(tj_render_backend_type backend_type); //loads the backend library
void tj_render_unload_backend(tj_render_backend_type backend_type); //Unloads the backends library

#endif
