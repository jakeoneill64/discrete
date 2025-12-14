#include "vulkan.h"

#include "spdlog/spdlog.h"
#include "log.h"

#include <vulkan/vulkan.h>
#include <string>
#include <format>

VKAPI_ATTR inline VkBool32 VKAPI_CALL vulkanDebugCallback(
VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
VkDebugUtilsMessageTypeFlagsEXT messageTypes,
const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
void* pUserData)
{

    std::string messageType =
    std::string{
        messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT      ? "[general] "     : ""} +
        (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  ? "[validation] "  : "") +
        (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ? "[performance] " : "");

    const auto level =
             messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ? spdlog::level::err :
             messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ? spdlog::level::warn :
             messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ? spdlog::level::info :
             messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT ? spdlog::level::debug :
             spdlog::level::info;

    const std::string message = std::format(
        "Vulkan {}: {}", messageType, pCallbackData->pMessage
        );

    log(
        std::format(
            "Vulkan {}: {}", messageType, pCallbackData->pMessage
            ),
            level
        );

    return VK_FALSE;
}

inline uint32_t vulkanFindMemoryIndex(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties,
        VkPhysicalDevice physicalDevice
        ) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & 1 << i &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
            }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

inline VkShaderModule createShaderPipelineStep(
        VkDevice device,
        const unsigned char* spirvData,
        const unsigned int spirvSize
) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvSize;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(spirvData);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }

    return shaderModule;
}
