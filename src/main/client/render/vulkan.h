#ifndef VULKAN_H
#define VULKAN_H

#include "spdlog/spdlog.h"
#include <vulkan/vulkan.h>

VKAPI_ATTR inline VkBool32 VKAPI_CALL vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
    );

template<typename VulkanTypeToEnumerate>
std::vector<VulkanTypeToEnumerate> vulkanEnumerateList(const std::function<void(uint32_t* count, VulkanTypeToEnumerate* data)> &vulkanEnumerator) {
    uint32_t count = 0;
    vulkanEnumerator(&count, nullptr);
    std::vector<VulkanTypeToEnumerate> result(count);
    vulkanEnumerator(&count, result.data());
    return result;
}

inline VkShaderModule createShaderPipelineStep(
        VkDevice device,
        const unsigned char* spirvData,
        const unsigned int spirvSize
);

inline uint32_t vulkanFindMemoryIndex(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties,
        VkPhysicalDevice physicalDevice
        );

#endif //VULKAN_H
