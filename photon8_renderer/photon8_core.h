#ifndef _PHOTON8_CORE_H
#define _PHOTON8_CORE_H

#include"photon8_structdefs.h"
#include<SDL2/SDL.h>
#include<SDL2/SDL_vulkan.h>


/*
    The code of the photon8_renderer will be done later, after completing tjlang part. 
*/

typedef struct photon8_core {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR render_surface;

    pn8_sysconnection system_connection;
    struct {
        u32 count;
        char** data;
    } extensions;
    struct {
        u32 count;
        VkLayerProperties* data;
    } layers;
    bool enabled_validation_layers;
} photon8_core;

void pn8_core_init(photon8_core* pn8_core,SDL_Window* window);
void pn8_core_setup_debug_messenger(photon8_core* pn8_core);
bool pn8_check_validation_layer_support(photon8_core* pn8_core);
bool pn8_is_physical_device_suitable();
void pn8_core_shutdown(photon8_core* pn8_core);

#endif
