#ifndef DISCRETE_RENDER_H
#define DISCRETE_RENDER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

struct RenderStartEvent{
    std::shared_ptr<World> world;
};

const std::vector<const char*> INSTANCE_EXTENSION = {
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME  // Required for ray tracing
};

#ifdef DISCRETE_DEBUG
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};
#endif

// Required device extensions for RTX
const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
};

#endif //DISCRETE_RENDER_H