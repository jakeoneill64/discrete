#define GLFW_INCLUDE_VULKAN

#include "glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "persistence/database.h"
#include "persistence/sqlite3.h"
#include "render/render.h"
#include "render/Shader.h"
#include "data/basic.frag.spv.h"
#include "data/basic.vert.spv.h"

#include <string>
#include <vector>
#include <ranges>
#include <iostream>
#include <span>

Client::Client()
:
    m_shouldRun{true},
    m_eventManager{std::make_shared<EventManager>()},
    m_database{[] {
          sqlite3* db = nullptr;
          if (sqlite3_open("discrete.db", &db) != SQLITE_OK) {
              throw std::runtime_error("Failed to open DB");
          }
          return std::shared_ptr<sqlite3>(db, [](sqlite3* ptr) {
              sqlite3_close(ptr);
          });
    }()},
    m_configRepository{m_eventManager, m_database},
    m_inputRepository{m_eventManager, m_database}
{

    sqlite3_update_hook(
        m_database.get(),
        dbEventCallback,
        &m_eventManager
    );

    if(!glfwInit()){
        throw std::runtime_error("GLFW failed to initialise");
    }

    // Ensure GLFW does NOT create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>(
            glfwCreateWindow(
            m_configRepository["client.window.initial_width"].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
            m_configRepository["client.window.initial_height"].transform(
                [](const auto version){
                    return std::stoi(version);
                }
            ).value_or(6),
            m_configRepository["client.window.name"].value_or("Discrete Engine").c_str(),
            nullptr,
            nullptr)
    );

    glfwMakeContextCurrent(m_window.get());
    glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow *window, int width, int height) {
        //TODO vulkan adjust viewport / surface
    });

#ifdef DISCRETE_DEBUG

    std::vector<VkLayerProperties> availableValidationLayers{64};
    std::uint32_t numberOfAvailableLayers;
    vkEnumerateInstanceLayerProperties(&numberOfAvailableLayers, availableValidationLayers.data());
    const auto availableValidationLayerNames = availableValidationLayers |
        std::views::transform([](const VkLayerProperties &layer){
            return layer.layerName;
        });

    std::for_each(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end(), [&availableValidationLayerNames](auto &layer){
        if(std::find(availableValidationLayerNames.begin(), availableValidationLayerNames.end(), layer) != availableValidationLayerNames.end())
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
    debugCreateInfo.pfnUserCallback = [](
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) -> VkBool32 {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    };

#endif

    uint32_t glfwExtensionCount{0};
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> allInstanceExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    allInstanceExtensions.insert(allInstanceExtensions.end(), INSTANCE_EXTENSION.begin(), INSTANCE_EXTENSION.end());

    VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,                 // VkStructureType sType;
        (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo,  // const void* pNext;
        // macos / moltenvk requires portability flag
        VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,       // VkInstanceCreateFlags flags;
        nullptr,                                                // const VkApplicationInfo* pApplicationInfo;
#ifdef DISCRETE_DEBUG
        static_cast<uint32_t>(VALIDATION_LAYERS.size()),
        VALIDATION_LAYERS.data(),
#else
        0,                                      // uint32_t enabledLayerNameCount;
        nullptr,                                // const char* const* ppEnabledLayerNames;
#endif
        static_cast<uint32_t>(allInstanceExtensions.size()),    // uint32_t enabledExtensionNameCount;
        allInstanceExtensions.data(),                           // const char* const* ppEnabledExtensionNames;
    };

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_vulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    std::vector<VkPhysicalDevice> physicalDevices = enumerateVulkanList<VkPhysicalDevice>(
        [&](uint32_t* count, VkPhysicalDevice* data){
            vkEnumeratePhysicalDevices(m_vulkanInstance, count, data);
        }
    );

    VkPhysicalDevice chosenDevice{physicalDevices[0]};
    for(const auto &physicalDevice : physicalDevices){
        // select based on: Discrete GPU, RTX capability, Compute Shaders, Memory Capacity, Speed
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);


    }

    std::vector<VkQueueFamilyProperties> deviceQueueFamilies = enumerateVulkanList<VkQueueFamilyProperties>(
        [&](uint32_t* count, VkQueueFamilyProperties* data){
            vkGetPhysicalDeviceQueueFamilyProperties(chosenDevice, count, data);
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

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *selectedQueueFamilyIndex,
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
            .enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size()),
            .ppEnabledExtensionNames = DEVICE_EXTENSIONS.data(),
            .pEnabledFeatures = nullptr,
    };


    VkDevice device;
    vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = *selectedQueueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    // TODO allocate one of these per frame and mark them as transient
    // https://kylemayes.github.io/vulkanalia/dynamic/recycling_command_buffers.html
    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_vulkanInstance, m_window.get(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan surface!");
    }

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(chosenDevice, surface, &capabilities);

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = 3; // triple buffered
    swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.imageExtent = capabilities.currentExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = createShaderPipelineStep(device, basic_vert_spv, basic_vert_spv_len);
    vertShaderStageInfo.pName = "main"; // Entry point of the shader

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = createShaderPipelineStep(device, basic_frag_spv, basic_frag_spv_len);
    fragShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkImage images[4]; uint32_t swapCount;
    //    vkGetSwapchainImagesKHR(dev, swap, &swapCount, images);


    // --- initialise engine ---

    // https://www.glfw.org/docs/3.3/input_guide.html
    glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
        instance().m_eventManager->publishEvent<KeyEvent>({key, action});
    });

    glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
        instance().m_eventManager->publishEvent<MousePositionEvent>({xPos, yPos});
    });

    glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
        instance().m_eventManager->publishEvent<MouseButtonEvent>({button, action, mods});
    });

    m_eventManager->subscribeEvent<KeyEvent>(
        [&](KeyEvent event){
        if(event.action != GLFW_REPEAT && event.action != GLFW_PRESS)
            return;
//        m_engine.submit(
//            m_inputRepository[GLFW_KEY_MAPPINGS.at(event.key),
//            m_engine.getInputContext(m_boundEntityId)]
//        );
    });

    m_eventManager->subscribeEvent<MouseButtonEvent>(
        [&](KeyEvent event) {
            if (event.action != GLFW_REPEAT && event.action != GLFW_PRESS)
                return;
//            m_engine.submit<void>(
//                    m_inputRepository[GLFW_MOUSE_BUTTON_MAPPINGS.at(event.key),
//                            m_engine.getInputContext(m_boundEntityId)]
//            );
        }
    );

    m_eventManager->subscribeEvent<MousePositionEvent>(
        [&](MousePositionEvent event) {
//            m_engine.submit<EntityLookParam>(
//                    std::make_unique<EntityLook>(EntityLook{{m_boundEntityId, event.xPos, event.yPos}})
//            );
        }
    );


}

void Client::loop() {
    while(m_shouldRun){
//        uint32_t currentSwapImage;
//        vkAcquireNextImageKHR(dev, swap, UINT64_MAX, presentCompleteSemaphore, NULL, &currentSwapImage);
//
//        VkImageView backbufferView;
//        vkCreateImageView(dev, &backbufferViewCreateInfo, NULL, &backbufferView);
//
//        VkQueue queue;
//        vkGetDeviceQueue(dev, 0, 0, &queue);
//
//        VkRenderPassCreateInfo renderpassCreateInfo = {
//
//        };
//
//        VkRenderPass renderpass;
//        vkCreateRenderPass(dev, &renderpassCreateInfo, NULL, &renderpass);
    }
}



void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
