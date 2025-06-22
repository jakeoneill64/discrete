#ifndef DISCRETE_RENDER_H
#define DISCRETE_RENDER_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "log.h"

#include <memory>
#include <vector>
#include <functional>
#include <format>

const std::vector<const char*> INSTANCE_EXTENSIONS = {
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_KHR_portability_subset",
};

#ifdef DISCRETE_DEBUG
const std::vector<const char*> VALIDATION_LAYERS {
    "VK_LAYER_KHRONOS_validation"
};

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
             (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ? spdlog::level::err :
             (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ? spdlog::level::warn :
             (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) ? spdlog::level::info :
             (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) ? spdlog::level::debug :
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

#endif

template<typename T>
std::vector<T> vulkanEnumerateList(const std::function<void(uint32_t* count, T* data)> &vulkanEnumerator) {
    uint32_t count = 0;
    vulkanEnumerator(&count, nullptr);
    std::vector<T> result(count);
    vulkanEnumerator(&count, result.data());
    return result;
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



struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
};


constexpr glm::vec3 vertices[] = {
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f}
};

constexpr glm::vec3 normals[] = {
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f}
};

constexpr uint16_t indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9,10,10,11, 8,
   12,13,14,14,15,12,
   16,17,18,18,19,16,
   20,21,22,22,23,20
};

constexpr glm::vec2 uvs[] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}
};




#endif //DISCRETE_RENDER_H