#include"photon8_structdefs.h"
#include"stdlib.h"

pn8_sysconnection pn8_sysconnection_create(VkInstance instance) {
    pn8_sysconnection sys_connection;

    u32 physical_devices_count;
    VkPhysicalDevice* physical_devices;

    vkEnumeratePhysicalDevices(instance, &physical_devices_count, physical_devices);
    physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_devices_count);
    vkEnumeratePhysicalDevices(instance, &physical_devices_count, physical_devices);
}


pn8_queue_family pn8_find_queue_families(VkPhysicalDevice physical_device){
    pn8_queue_family queue_family;
    int i = 0;

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
    VkQueueFamilyProperties* queue_family_properties = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

    for(;i < queue_family_count; i++) {
        if(queue_family_properties[i].queueFlags && VK_QUEUE_GRAPHICS_BIT) {
            queue_family.graphics_qfamily_index.i = i;
            queue_family.graphics_qfamily_index.valid = 1;
        }

        if(pn8_queue_family_is_complete(queue_family)){
            break;
        }
        i++;
    }

    return queue_family;

}

bool is_physical_device_suitable(VkPhysicalDevice physical_device){
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

    return physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        physical_device_features.geometryShader;
}

void pn8_sysconnection_destroy(pn8_sysconnection* sys_connection) {
}
