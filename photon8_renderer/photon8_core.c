#include"photon8_core.h"
#include<stdio.h>
#include<string.h>

#include"stdlib.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* pUserData) {
        printf("VALIDAITON LAYER: %s\n", callback_data->pMessage);
 
    return VK_FALSE;
}

void pn8_core_init(photon8_core* pn8_core, SDL_Window* window) {
    pn8_core->enabled_validation_layers = 0;

    #ifdef _VK_DEBUG
    pn8_core->enabled_validation_layers = 1;
    #endif

    SDL_Vulkan_GetInstanceExtensions(window, &pn8_core->extensions.count, NULL);
    pn8_core->extensions.count += pn8_core->enabled_validation_layers;
    pn8_core->extensions.data = malloc(512 * pn8_core->extensions.count);
    SDL_Vulkan_GetInstanceExtensions(window, &pn8_core->extensions.count, pn8_core->extensions.data);

    if(pn8_core->enabled_validation_layers){
        if(!pn8_check_validation_layer_support(pn8_core))
            printf("validaiton layers not supported\n");
        pn8_core->extensions.data[pn8_core->extensions.count - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }else{
        pn8_core->layers.count = 0;
        pn8_core->layers.data = NULL;
    }

    VkInstanceCreateInfo instance_create_info = (VkInstanceCreateInfo){
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = 0,
        .ppEnabledExtensionNames = pn8_core->extensions.data,
        .enabledExtensionCount = pn8_core->extensions.count,
        .ppEnabledLayerNames = pn8_core->layers.data,
        .enabledLayerCount = pn8_core->layers.count,
        .pNext = NULL,
        .pApplicationInfo = NULL,
    };
    vkCreateInstance(&instance_create_info, NULL, &pn8_core->instance);

    pn8_core_setup_debug_messenger(pn8_core);


    pn8_core->system_connection = pn8_sysconnection_create(pn8_core->instance);
}

void pn8_core_setup_debug_messenger(photon8_core* pn8_core){

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info;
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = _debug_callback;
    debug_messenger_create_info.pUserData = NULL;

    PFN_vkCreateDebugUtilsMessengerEXT vk_create_debug_utils = vkGetInstanceProcAddr(pn8_core->instance, "vkCreateDebugUtilsMessengerEXT");
    if(vk_create_debug_utils == NULL) {
        printf("debbuger extension not present!\n");
        return;
    }

    if(!vk_create_debug_utils(pn8_core->instance,&debug_messenger_create_info,NULL,&pn8_core->debug_messenger)){
        printf("failed to create debug messenger\n");
    }
}

bool pn8_check_validation_layer_support(photon8_core* pn8_core) {
    pn8_core->layers.count = 0;
    vkEnumerateInstanceLayerProperties(&pn8_core->layers.count, NULL);
    pn8_core->layers.data = NULL;
    vkEnumerateInstanceLayerProperties(&pn8_core->layers.count, pn8_core->layers.data);

    char* required_layers[1] = {
        "VK_LAYER_KHORONOS_validation"
    };

    for(u32 i = 0; i < 1; i++) {
        bool layer_found = 0;

        for(u32 j = 0;j < pn8_core->layers.count; j++) {
            if(strncmp(pn8_core->layers.data[j].layerName,required_layers[i],128) == 0){
                layer_found = 1;
                break;
            }
        }

        if(!layer_found) {
            return 0;
        }
    }

    return 1;
}

void pn8_core_shutdown(photon8_core* pn8_core) {
    pn8_sysconnection_destroy(&pn8_core->system_connection);
    vkDestroyInstance(pn8_core->instance, NULL);
}
 