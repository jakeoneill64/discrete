#include "Renderer.h"
#include "client/render/VulkanRenderer.h"
#include "client/render/vulkan.h"
#include "GLFW/glfw3.h"
#include "engine/Engine.h"
#include "persistence/database.h"
#include "log.h"
#include "glm/glm.hpp"

#include <vulkan/vulkan.h>
#include <ranges>


PartialVulkanRenderer::PartialVulkanRenderer(const std::function<std::expected<JsonNode, JsonNodeError>()>& fetchConfig) {

    const auto currentConfig = fetchConfig();

    std::vector<const char*> validationLayers = currentConfig
        .and_then([](JsonNode& node) {
            return node["vulkan.device_extensions"];
        })
        .and_then([](const JsonNode& node) {
            return node.as<std::vector<std::string>>();
        })
       .value_or({"VK_LAYER_KHRONOS_validation"});

    std::vector<const char*> deviceExtensions = currentConfig
        .and_then([](JsonNode& node) {
            return node["vulkan.device_extensions"];
        })
        .and_then([](const JsonNode& node) {
            return node.as<std::vector<std::string>>();
        })
       .value_or({
           "VK_KHR_spirv_1_4",
           "VK_KHR_shader_float_controls",
           "VK_KHR_swapchain",
           "VK_KHR_portability_subset"
       });

    std::vector<const char*> instanceExtensions = currentConfig
        .and_then([](JsonNode& node) {
            return node["vulkan.instance_extensions"];
        })
        .and_then([](const JsonNode& node) {
            return node.as<std::vector<std::string>>();
        })
       .value_or({
           "VK_KHR_get_physical_device_properties2",
           "VK_KHR_portability_enumeration",
           "VK_EXT_debug_utils"
       });

    const auto availableValidationLayerNames = vulkanEnumerateList<VkLayerProperties>(
        []
            (uint32_t* count, VkLayerProperties* data){
            vkEnumerateInstanceLayerProperties(count, data);
        }) |
        std::views::transform([](const VkLayerProperties &layer){
            return std::string_view{layer.layerName};
        });

    // TODO do we really want to throw if the validation layer's not available?
    std::ranges::for_each(validationLayers, [&availableValidationLayerNames](auto &layer){
        if(std::find(availableValidationLayerNames.begin(), availableValidationLayerNames.end(), layer) == availableValidationLayerNames.end())
            throw std::runtime_error(std::string("Validation layer ") + layer + " not available.");
    });

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = vulkanDebugCallback;

    uint32_t glfwExtensionCount{0};
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> allInstanceExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    allInstanceExtensions.insert(allInstanceExtensions.end(), instanceExtensions.begin(), instanceExtensions.end());

    VkApplicationInfo applicationInfo = {
        .apiVersion =VK_API_VERSION_1_1, // TODO config item
        .pApplicationName = "Discrete Engine" // TODO config item
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,                // VkStructureType sType;
#ifdef DISCRETE_DEBUG
        (&debugCreateInfo),                                    // const void* pNext;
#elif
        nullptr,
#endif
        // macos / moltenvk requires portability flag
        VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,       // VkInstanceCreateFlags flags;
        &applicationInfo,                                                // const VkApplicationInfo* pApplicationInfo;
#ifdef DISCRETE_DEBUG
        static_cast<uint32_t>(validationLayers.size()),
        validationLayers.data(),
#else
        0,                                      // uint32_t enabledLayerNameCount;
        nullptr,                                // const char* const* ppEnabledLayerNames;
#endif
        static_cast<uint32_t>(allInstanceExtensions.size()),    // uint32_t enabledExtensionNameCount;
        allInstanceExtensions.data(),                           // const char* const* ppEnabledExtensionNames;
    };

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    VkDebugUtilsMessengerEXT debugMessenger;
    auto func = static_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func) {
        if (func(m_instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    std::vector<VkPhysicalDevice> physicalDevices = vulkanEnumerateList<VkPhysicalDevice>(
        [&](uint32_t* count, VkPhysicalDevice* data){
            vkEnumeratePhysicalDevices(m_instance, count, data);
        }
    );

    m_physicalDevice = physicalDevices[0];
    for(const auto &physicalDevice : physicalDevices){
        // TODO select based on:
        // Discrete GPU, RTX capability, Compute Shaders, Memory Capacity, Speed
        // as well as support for our enabled features features
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
        accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelStructFeatures.accelerationStructure = VK_TRUE;

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
        rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
        descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.features = deviceFeatures;

        bufferDeviceAddressFeatures.pNext = &accelStructFeatures;
        accelStructFeatures.pNext = &rayTracingPipelineFeatures;
        rayTracingPipelineFeatures.pNext = &descriptorIndexingFeatures;
        deviceFeatures2.pNext = &bufferDeviceAddressFeatures;

        vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures2);
        // now we can check for these non-core feature support with something like
        // rayTracingPipelineFeatures.rayTracingPipeline == VK_TRUE

        std::vector<VkExtensionProperties> deviceExtensionProperties = vulkanEnumerateList<VkExtensionProperties>(
            [physicalDevice]
            (uint32_t* count, auto data)
            {
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, count, data);
            });

    }

    std::vector<VkQueueFamilyProperties> deviceQueueFamilies = vulkanEnumerateList<VkQueueFamilyProperties>(
        [&](uint32_t* count, VkQueueFamilyProperties* data){
            vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, count, data);
        }
    );

    std::optional<uint32_t> selectedQueueFamilyIndex;
    for (uint32_t i = 0; i < deviceQueueFamilies.size(); ++i) {
        if (deviceQueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            selectedQueueFamilyIndex = i;
            break;
        }
    }

    if(!selectedQueueFamilyIndex){
        throw std::runtime_error("No queue families support graphics operations");
    }

    m_graphicsQueueFamilyIndex = *selectedQueueFamilyIndex;

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = m_graphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;


    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
    bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
    accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    accelStructFeatures.accelerationStructure = VK_TRUE;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
    rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
    descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
    descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.features = deviceFeatures;

    bufferDeviceAddressFeatures.pNext = &accelStructFeatures;
    accelStructFeatures.pNext = &rayTracingPipelineFeatures;
    rayTracingPipelineFeatures.pNext = &descriptorIndexingFeatures;
    deviceFeatures2.pNext = &bufferDeviceAddressFeatures;

    VkDeviceCreateInfo deviceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &deviceFeatures2,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = nullptr,
    };

    if (vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create device");
    }

    VkQueue graphicsQueue;
    vkGetDeviceQueue(m_device, *selectedQueueFamilyIndex, 0, &graphicsQueue);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = *selectedQueueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
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