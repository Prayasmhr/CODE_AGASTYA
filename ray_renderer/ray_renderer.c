
#ifdef _WIN32
    #define RAY_RENDERER_API __declspec(dllexport)
#else
    #error "unsupported platform"
#endif

#include"../vendor/SDL2/SDL.h"
#include"../vendor/SDL2/SDL_ttf.h"
#include"../vendor/SDL2/SDL_image.h"
#include"../teja_ui/tj_renderer.h"
#include"../teja_ui/tj_asset.h"
#include"stdarg.h"
#include<stdio.h>

struct tj_font {
    TTF_Font* font;
    u16 size;
};

struct tj_image {
    i32 width;
    i32 height;
    SDL_Texture* texture;
    SDL_Surface* surface;
};

struct tj_render_backend {
   SDL_Window* window;
   SDL_Renderer* renderer; 
};

typedef tj_render_backend ray_render_backend;
tj_render_engine render_engine;


RAY_RENDERER_API bool ray_renderer_init(tj_render_config render_create_specs){
    printf("intializing ray renderer\n");
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    render_engine.config.window_width = render_create_specs.window_width;
    render_engine.config.window_height = render_create_specs.window_height;
    render_engine.config.logical_width = render_create_specs.logical_width;
    render_engine.config.logical_height = render_create_specs.logical_height;
    render_engine.config.title = render_create_specs.title;

    render_engine.render_backend = malloc(sizeof(ray_render_backend));

    render_engine.render_backend->window = SDL_CreateWindow(
        render_create_specs.title.data,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        render_engine.config.window_width,
        render_engine.config.window_height,
        SDL_WINDOW_RESIZABLE
    );
    render_engine.render_backend->renderer = SDL_CreateRenderer(render_engine.render_backend->window, -1, 0);
    SDL_RenderSetLogicalSize(render_engine.render_backend->renderer, render_engine.config.logical_width, render_engine.config.logical_height);

    string_free(&render_create_specs.title); 
    return 1;
}


RAY_RENDERER_API tj_image* ray_renderer_create_image(char* path){
    tj_image* out = malloc(sizeof(tj_image));
    out->surface = IMG_Load(path);
    if(!out->surface){
        printf("image error: %s", IMG_GetError());
    }

    printf("creating texture from the loaded surface (loaded from disk %s)\n",path);
    out->texture = SDL_CreateTextureFromSurface(render_engine.render_backend->renderer,out->surface);
    if(!out->texture) {
        printf("failed to create texture SDL_error: %s\n", SDL_GetError());
    }

    if(SDL_QueryTexture(out->texture, 0, 0, &out->width, &out->height)) {
        printf("failed to query texture SDL_error: %s\n", SDL_GetError());
    }
    
    return out;
}

RAY_RENDERER_API void ray_renderer_render_image(tj_image* image,tj_vec2i pos){
    SDL_Rect destination = (SDL_Rect){.x = pos.x,.y = pos.y, image->width, image->height};
    SDL_RenderCopy(render_engine.render_backend->renderer, image->texture, NULL, &destination);
}

RAY_RENDERER_API void ray_renderer_set_image_size(tj_image* image,tj_vec2i dimensions) {
    image->width = dimensions.x;
    image->height = dimensions.y;    
}

RAY_RENDERER_API void ray_renderer_set_window_size( i32 window_width, i32 window_height){
    render_engine.config.window_width = window_width;
    render_engine.config.window_height = window_height;
}

RAY_RENDERER_API void ray_renderer_render_text(tj_font* font,tj_color fg, tj_color bg, tj_vec2i pos,u32 max,char* fmt,...){
    char text_buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(text_buffer, max, fmt, args);
    va_end(args);

    SDL_Surface* text_surface = TTF_RenderText_Shaded(
        font->font,text_buffer,
        (SDL_Color) { fg.R, fg.G, fg.B, fg.A},
        (SDL_Color) { bg.R, bg.G, bg.B, bg.A}
    ); 
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(render_engine.render_backend->renderer, text_surface);

    SDL_Rect destination = (SDL_Rect){.x = pos.x,.y = pos.y};
    SDL_QueryTexture(text_texture, 0, 0, &destination.w, &destination.h);
    SDL_RenderCopy(render_engine.render_backend->renderer, text_texture, NULL, &destination);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

RAY_RENDERER_API void ray_renderer_render_circle(tj_vec2i center, u8 radius, tj_color color) {
    u32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    u32 pixel_color = (color.R << 24) + (color.G << 16) + (color.B << 8) + 255; //i quite dont understand both of these calculations but it is based on how the system handle pixels and its byte order. 
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    u32 pixel_color = (color.A << 24) + (color.B << 16) + (color.G << 8) + color.R;
    #endif

    /*
        --- In big endian system , the most signficant byte comes first, red is the highest bits. In a 32-bit color value it would be [R][G][B][A]
        --- In little endian system, the least signficant byte comes first, red is the lowest bits. Ina 32-bit color value it would be [A][B][G][R]


        Endianess refers to the order in which bytes are arrranged to form larger numerical values in computer memory.
            --Big Endian-Most significant byte stores in the lowest memory address (i.e. comes first)
            --Little Endian- Least signficiant byte stores in the lowest meomory address (i.e. comes first)
    */

    SDL_Rect circle_bounds = (SDL_Rect) {
        .x = center.x - radius,
        .y = center.y - radius,
        .w = radius * 2,
        .h = radius * 2
    };

    SDL_Surface* circle_surface = SDL_CreateRGBSurface(0, circle_bounds.w, circle_bounds.h, 32, rmask, gmask, bmask, amask);
    SDL_Texture* circle_texture = SDL_CreateTextureFromSurface(render_engine.render_backend->renderer, circle_surface);
    SDL_FreeSurface(circle_surface);

    u32* circle_pixels = malloc(sizeof(u32) * circle_bounds.w * circle_bounds.h); //typically storing the 2D pixel array into a 1D array
    memset(circle_pixels, 0, sizeof(u32) * circle_bounds.w * circle_bounds.h);

    i32 clamped_x = center.x - circle_bounds.x;
    i32 clamped_y = center.y - circle_bounds.y;
    i32 square_area = radius * radius;

    for(i32 x = 0;x < circle_bounds.w; ++x) {
        for(i32 y = 0;y < circle_bounds.h; ++y) {
            i32 circle_v = (x - clamped_x) * (x - clamped_x) + (y - clamped_y) * (y - clamped_y) - square_area;
            if(circle_v <= 0) {
                circle_pixels[(y * circle_bounds.w) + x] = pixel_color; //see important concepts file to understand this thing
            }
        }
    }
    SDL_UpdateTexture(circle_texture, NULL, circle_pixels, circle_bounds.w * sizeof(u32));

    SDL_RenderCopy(render_engine.render_backend->renderer, circle_texture, NULL, &circle_bounds);

    free(circle_pixels);
    SDL_DestroyTexture(circle_texture);
}

RAY_RENDERER_API void ray_renderer_render_rect(tj_rectangle rectangle, tj_color color, u8 corner_radius) {
    SDL_Rect rect = (SDL_Rect){ .x = rectangle.x + corner_radius ,.y = rectangle.y,.w = rectangle.w - corner_radius,.h = rectangle.h };

    SDL_SetRenderDrawColor(render_engine.render_backend->renderer, color.R , color.G, color.B, color.A);
    SDL_RenderFillRect(render_engine.render_backend->renderer, &rect);

    SDL_SetRenderDrawColor(render_engine.render_backend->renderer, color.R + 1, color.G, color.B, color.A);
    rect = (SDL_Rect){ .x = rectangle.x ,.y = rectangle.y + corner_radius,.w = rectangle.w,.h = rectangle.h - corner_radius};
    SDL_RenderFillRect(render_engine.render_backend->renderer, &rect);

    if(corner_radius > 0) {
        // ray_renderer_render_circle((tj_vec2i) {.x = rectangle.x + corner_radius,.y = rectangle.y + corner_radius}, corner_radius, color);
        // ray_renderer_render_circle((tj_vec2i) {.x = rectangle.w - corner_radius,.y = rectangle.h - corner_radius}, corner_radius, color);
        // ray_renderer_render_circle((tj_vec2i) {.x = rectangle.w - corner_radius,.y = rectangle.y + corner_radius}, corner_radius, color);
        // ray_renderer_render_circle((tj_vec2i) {.x = rectangle.x + corner_radius,.y = rectangle.h - corner_radius}, corner_radius, color);
    }
}

RAY_RENDERER_API void ray_renderer_render_line(tj_vec2i start,tj_vec2i end,tj_color color) {
    SDL_SetRenderDrawColor(render_engine.render_backend->renderer, color.R, color.G, color.B, color.A);
    SDL_RenderDrawLine(render_engine.render_backend->renderer, start.x, start.y, end.x, end.y);
}

RAY_RENDERER_API void ray_renderer_clear(tj_color color){
    SDL_SetRenderDrawColor(render_engine.render_backend->renderer, color.R, color.G, color.B, color.A);
    SDL_RenderClear(render_engine.render_backend->renderer);
}

RAY_RENDERER_API void ray_renderer_flip(void){
    SDL_RenderPresent(render_engine.render_backend->renderer);
}

RAY_RENDERER_API void ray_renderer_destroy_image(tj_image* image) {
    SDL_DestroyTexture(image->texture);
}

RAY_RENDERER_API void ray_renderer_shutdown(void){
    SDL_DestroyRenderer(render_engine.render_backend->renderer);
    SDL_DestroyWindow(render_engine.render_backend->window);
}

RAY_RENDERER_API tj_font* ray_renderer_create_font(char* path,u16 size){
    printf("loading font from disk %s\n",path);
    tj_font* out = malloc(sizeof(tj_font));
    out->size = size;
    out->font = TTF_OpenFont(path,size); 
    if(!out->font){
        printf("error openning font: %s", TTF_GetError());
    }
    return out;
}

RAY_RENDERER_API void ray_renderer_destroy_font(tj_font* self){
    TTF_CloseFont(self->font);
}

RAY_RENDERER_API tj_vec2i ray_renderer_get_font_size(tj_font* self,const char* text){
    tj_vec2i size;
    TTF_SizeText(self->font, text, &size.x, &size.y);
    return size;
}

RAY_RENDERER_API tj_vec2i ray_renderer_get_image_size(tj_image* image) {
    printf("getting image size\n");
    tj_vec2i size;
    SDL_QueryTexture(image->texture, 0, 0, &image->width, &image->height);
    printf("query texture %d %d\n", image->width, image->height);
    size.x = image->width; 
    size.y = image->height;
    return size;
}
