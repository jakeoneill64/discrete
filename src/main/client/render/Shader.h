#ifndef DISCRETE_SHADER_H
#define DISCRETE_SHADER_H

#include "vulkan/vulkan.h"
#include <span>

inline VkShaderModule createShaderPipelineStep(VkDevice device, const std::span<uint32_t>& spirvContent) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvContent.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(spirvContent.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }

    return shaderModule;
}

#endif //DISCRETE_SHADER_H
