#ifndef _PHOTON8_STRUCTDEFS_H
#define _PHOTON8_STRUCTDEFS_H

#include<vulkan/vulkan.h>
#include"../heart_lib/ht_defines.h"

typedef struct pn8_queue_family{
    optional(u32,i) graphics_qfamily_index;
    optional(u32,i) present_qfamily_index;
} pn8_queue_family;

typedef struct pn8_sysconnection {
    VkPhysicalDevice physical_device;
    VkDevice device;
} pn8_sysconnection;

inline bool pn8_queue_family_is_complete(pn8_queue_family queue_family) { queue_family.graphics_qfamily_index.valid && queue_family.present_qfamily_index.valid;}

pn8_sysconnection pn8_sysconnection_create(VkInstance instance);
bool is_physical_device_suitable(VkPhysicalDevice physical_device);
void pn8_sysconnection_destroy(pn8_sysconnection* sys_connection);

#endif