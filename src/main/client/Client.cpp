#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include "Client.h"
#include "input/input.h"
#include "persistence/database.h"
#include "persistence/config.h"
#include "sqlite3.h"
#include "render/render.h"
#include "render/Shader.h"
#include "data/basic.frag.spv.h"
#include "data/basic.vert.spv.h"
#include "log.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>
#include <vector>
#include <ranges>
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
    }()}
{

    LoggingContext::setLoggerName("client");

    sqlite3_update_hook(
        m_database.get(),
        dbEventCallback,
        &m_eventManager
    );

    if(!glfwInit()){
        throw std::runtime_error("GLFW failed to initialise");
    }

    // Ensure GLFW doesn't create a openGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = std::unique_ptr<GLFWwindow, DestroyGLFWWindow>{
            glfwCreateWindow(
            getRecord<ConfigEntry>(m_database.get(), std::string{"key"}, std::string{"client.window.initial_width"}).transform(
                [](const auto& entry){
                    return std::stoi(entry.value);
                }
            ).value_or(800),
            getRecord<ConfigEntry>(m_database.get(), std::string{"key"}, std::string{"client.window.initial_height"}).transform(
                [](const auto& entry){
                    return std::stoi(entry.value);
                }
            ).value_or(600),
            getRecord<ConfigEntry>(m_database.get(), std::string{"key"}, std::string{"client.window.name"}).transform(
                [](const auto& entry){
                    return entry.value.c_str();
                }
            ).value_or("Discrete Engine"),
            nullptr,
            nullptr)
    };

    glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow *window, int width, int height) {
        //TODO vulkan adjust viewport / surface
    });

#ifdef DISCRETE_DEBUG

    const auto availableValidationLayerNames = vulkanEnumerateList<VkLayerProperties>(
        []
            (uint32_t* count, VkLayerProperties* data){
            vkEnumerateInstanceLayerProperties(count, data);
        }) |
        std::views::transform([](const VkLayerProperties &layer){
            return std::string_view{layer.layerName};
        });

    std::ranges::for_each(VALIDATION_LAYERS, [&availableValidationLayerNames](auto &layer){
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

#endif

    uint32_t glfwExtensionCount{0};
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> allInstanceExtensions{glfwExtensions, glfwExtensions + glfwExtensionCount};
    allInstanceExtensions.insert(allInstanceExtensions.end(), INSTANCE_EXTENSIONS.begin(), INSTANCE_EXTENSIONS.end());

    VkApplicationInfo applicationInfo = {
        .apiVersion =VK_API_VERSION_1_1,
        .pApplicationName = "Discrete Engine"
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

    VkDebugUtilsMessengerEXT debugMessenger;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
    if (func) {
        if (func(m_vulkanInstance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }


    std::vector<VkPhysicalDevice> physicalDevices = vulkanEnumerateList<VkPhysicalDevice>(
        [&](uint32_t* count, VkPhysicalDevice* data){
            vkEnumeratePhysicalDevices(m_vulkanInstance, count, data);
        }
    );

    VkPhysicalDevice chosenDevice{physicalDevices[0]};
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
    if (vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create device");
    }

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, *selectedQueueFamilyIndex, 0, &graphicsQueue);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = *selectedQueueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    // TODO allocate one of these per frame and mark them as transient
    // https://kylemayes.github.io/vulkanalia/dynamic/recycling_command_buffers.html
    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(m_vulkanInstance, m_window.get(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan surface!");
    }

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(chosenDevice, surface, &capabilities);

    VkSwapchainCreateInfoKHR swapchainInfo{
        .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext            = nullptr,
        .flags            = 0,
        .surface          = surface,
        .minImageCount    = 3,
        .imageFormat      = VK_FORMAT_B8G8R8A8_UNORM,
        .imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent      = capabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform     = capabilities.currentTransform,
        .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode      = VK_PRESENT_MODE_FIFO_KHR,
        .clipped          = VK_TRUE,
        .oldSwapchain     = VK_NULL_HANDLE
    };


    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = createShaderPipelineStep(device, basic_vert_spv, basic_vert_spv_len);
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = createShaderPipelineStep(device, basic_frag_spv, basic_frag_spv_len);
    fragShaderStageInfo.pName = "main";

    std::array shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

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

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);

    std::array<VkVertexInputBindingDescription, 2> bindingDescriptions{};

    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions[1].binding = 1;
    bindingDescriptions[1].stride = sizeof(glm::mat4);
    bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offset_of(&Vertex::position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offset_of(&Vertex::normal);

    for (uint32_t i = 0; i < 4; ++i) {
        attributeDescriptions[2 + i].binding = 1;
        attributeDescriptions[2 + i].location = 2 + i;
        attributeDescriptions[2 + i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2 + i].offset = sizeof(glm::vec4) * i;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(capabilities.currentExtent.width);
    viewport.height = static_cast<float>(capabilities.currentExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = capabilities.currentExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkDescriptorSetLayoutBinding matricesLayoutBinding{};
    matricesLayoutBinding.binding = 0;
    matricesLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    matricesLayoutBinding.descriptorCount = 1;
    matricesLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    matricesLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &matricesLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet);

    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    VkBufferCreateInfo uniformBufferInfo{};
    uniformBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    uniformBufferInfo.size = 2 * sizeof(glm::mat4);
    uniformBufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    uniformBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &uniformBufferInfo, nullptr, &uniformBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create index buffer");
    }

    VkMemoryRequirements uniformBufferMemoryRequirements;
    vkGetBufferMemoryRequirements(device, uniformBuffer, &uniformBufferMemoryRequirements);

    uint32_t uboMemoryTypeIndex = vulkanFindMemoryIndex(
        uniformBufferMemoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevices[0]
    );

    VkMemoryAllocateInfo uniformBufferMemoryAllocateInfo{};
    uniformBufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    uniformBufferMemoryAllocateInfo.allocationSize = uniformBufferMemoryRequirements.size;
    uniformBufferMemoryAllocateInfo.memoryTypeIndex = uboMemoryTypeIndex;

    if (vkAllocateMemory(device, &uniformBufferMemoryAllocateInfo, nullptr, &uniformBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate index buffer memory");
    }

    vkBindBufferMemory(device, uniformBuffer, uniformBufferMemory, 0);

    void* uniformData;

    glm::mat4 viewMatrix = glm::lookAt(
    glm::vec3(0.0f, 3.0f, 5.0f),
    glm::vec3(1.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(capabilities.currentExtent.width) / static_cast<float>(capabilities.currentExtent.height),
        0.1f,
        100.0f
    );

    std::array<glm::mat4, 2> matrices{viewMatrix, projectionMatrix};
    vkMapMemory(device, uniformBufferMemory, 0, 2 * sizeof(glm::mat4), 0, &uniformData);
    memcpy(uniformData, matrices.data(), 2 * sizeof(glm::mat4));
    vkUnmapMemory(device, uniformBufferMemory);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4) * 2;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;       // the descriptor set you allocated
    descriptorWrite.dstBinding = 0;                // matches binding in your shader
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);


    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.subpass = 0;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;

    VkPipeline graphicsPipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);

    // TODO handle resizing. We need to destroy and recreate all images / imageviews.
    std::vector<VkImage> swapchainImages = vulkanEnumerateList<VkImage>(
        [&](uint32_t* count, VkImage* data){
            vkGetSwapchainImagesKHR(device, swapchain, count, data);
        }
    );

    std::vector<VkImageView> swapchainImageViews{swapchainImages.size()};
    std::vector<VkImage> depthImages(swapchainImages.size());
    std::vector<VkDeviceMemory> depthImageMemories(swapchainImages.size());
    std::vector<VkImageView> depthImageViews(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
    {
        VkImageViewCreateInfo colorViewInfo{};
        colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorViewInfo.image = swapchainImages[i];
        colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;

        colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorViewInfo.subresourceRange.baseMipLevel = 0;
        colorViewInfo.subresourceRange.levelCount = 1;
        colorViewInfo.subresourceRange.baseArrayLayer = 0;
        colorViewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &colorViewInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create color image view!");
        }

        VkImageCreateInfo depthImageInfo{};
        depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageInfo.format = VK_FORMAT_D32_SFLOAT;
        depthImageInfo.extent.width = capabilities.currentExtent.width;
        depthImageInfo.extent.height = capabilities.currentExtent.height;
        depthImageInfo.extent.depth = 1;
        depthImageInfo.mipLevels = 1;
        depthImageInfo.arrayLayers = 1;
        depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (vkCreateImage(device, &depthImageInfo, nullptr, &depthImages[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create depth image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, depthImages[i], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = vulkanFindMemoryIndex(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            physicalDevices[0]
        );

        if (vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemories[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate depth image memory!");
        }

        vkBindImageMemory(device, depthImages[i], depthImageMemories[i], 0);

        VkImageViewCreateInfo depthViewInfo{};
        depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewInfo.image = depthImages[i];
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = VK_FORMAT_D32_SFLOAT;
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &depthViewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create depth image view!");
        }
    }

    std::vector<VkFramebuffer> swapchainFramebuffers;

    for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
        std::array frameBufferAttachments = {
            swapchainImageViews[i],
            depthImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = frameBufferAttachments.data();
        framebufferInfo.width = capabilities.currentExtent.width;
        framebufferInfo.height = capabilities.currentExtent.height;
        framebufferInfo.layers = 1;

        VkFramebuffer framebuffer;
        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }

        swapchainFramebuffers.push_back(framebuffer);
    }

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore imageAvailableSemaphore;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphore!");
    }


    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device,
        swapchain,
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swapchain image!");
    }

    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapchainFramebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = capabilities.currentExtent;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    std::vector<Vertex> vertexVector = [&]
    {
        auto view = std::views::iota(0u, static_cast<unsigned>(std::size(vertices)))
            | std::views::transform([&](unsigned int i) {
                return Vertex{ vertices[i], normals[i] };
            });
        return std::vector(view.begin(), view.end());
    }();

    auto instanceMatrix{glm::mat4(1.0f)};

    VkBuffer vertexBuffer;
    VkBufferCreateInfo vertexBufferInfo{};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size = sizeof(Vertex) * vertexVector.size();
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &vertexBufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    VkMemoryRequirements vertexMemoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &vertexMemoryRequirements);

    uint32_t vertexMemoryTypeIndex = vulkanFindMemoryIndex(
        vertexMemoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevices[0]
    );

    VkMemoryAllocateInfo vertexBufferAllocateInfo{};
    vertexBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vertexBufferAllocateInfo.allocationSize = vertexMemoryRequirements.size;
    vertexBufferAllocateInfo.memoryTypeIndex = vertexMemoryTypeIndex;

    VkDeviceMemory vertexBufferMemory;

    if (vkAllocateMemory(device, &vertexBufferAllocateInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void* mappedData;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(Vertex) * vertexVector.size(), 0, &mappedData);
    memcpy(mappedData, vertexVector.data(), sizeof(Vertex) * vertexVector.size());
    vkUnmapMemory(device, vertexBufferMemory);

    VkBuffer instanceBuffer;
    VkBufferCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    instanceInfo.size = sizeof(glm::mat4);
    instanceInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    instanceInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &instanceInfo, nullptr, &instanceBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    VkMemoryRequirements instanceMemoryRequirements;
    vkGetBufferMemoryRequirements(device, instanceBuffer, &instanceMemoryRequirements);

    uint32_t memoryTypeIndex = vulkanFindMemoryIndex(
        instanceMemoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevices[0]
    );

    VkMemoryAllocateInfo instanceMemoryAllocateInfo{};
    instanceMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    instanceMemoryAllocateInfo.allocationSize = instanceMemoryRequirements.size;
    instanceMemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory instanceBufferMemory;

    if (vkAllocateMemory(device, &instanceMemoryAllocateInfo, nullptr, &instanceBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(device, instanceBuffer, instanceBufferMemory, 0);

    void* mappedInstanceData;
    vkMapMemory(device, instanceBufferMemory, 0, sizeof(glm::mat4), 0, &mappedInstanceData);
    memcpy(mappedInstanceData, &instanceMatrix, sizeof(glm::mat4));
    vkUnmapMemory(device, instanceBufferMemory);

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkBufferCreateInfo indexBufferInfo{};
    indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexBufferInfo.size = sizeof(indices);
    indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &indexBufferInfo, nullptr, &indexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create index buffer");
    }

    VkMemoryRequirements indexMemRequirements;
    vkGetBufferMemoryRequirements(device, indexBuffer, &indexMemRequirements);

    uint32_t indexMemoryTypeIndex = vulkanFindMemoryIndex(
        indexMemRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevices[0]
    );

    VkMemoryAllocateInfo indexAllocInfo{};
    indexAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    indexAllocInfo.allocationSize = indexMemRequirements.size;
    indexAllocInfo.memoryTypeIndex = indexMemoryTypeIndex;

    if (vkAllocateMemory(device, &indexAllocInfo, nullptr, &indexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate index buffer memory");
    }

    vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);

    void* indexData;
    vkMapMemory(device, indexBufferMemory, 0, sizeof(indices), 0, &indexData);
    memcpy(indexData, indices, sizeof(indices));
    vkUnmapMemory(device, indexBufferMemory);

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = { vertexBuffer, instanceBuffer };
    VkDeviceSize offsets[] = { 0, 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(
    commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    pipelineLayout,           // Make sure this is the one used when creating the pipeline
    0,                        // First set
    1,                        // Descriptor set count
    &descriptorSet,           // Your VkDescriptorSet for set 0
    0, nullptr                // Dynamic offsets (if needed)
);
    vkCmdDrawIndexed(commandBuffer, 36, 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    // --- initialise engine ---

    // https://www.glfw.org/docs/3.3/input_guide.html
    // glfwSetKeyCallback(m_window.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
    //     instance().m_eventManager->publishEvent<KeyEvent>({key, action});
    // });
    //
    // glfwSetCursorPosCallback(m_window.get(), [](GLFWwindow* window, double xPos, double yPos){
    //     instance().m_eventManager->publishEvent<MousePositionEvent>({xPos, yPos});
    // });
    //
    // glfwSetMouseButtonCallback(m_window.get(), [](GLFWwindow* window, int button, int action, int mods){
    //     instance().m_eventManager->publishEvent<MouseButtonEvent>({button, action, mods});
    // });

    m_eventManager->subscribeEvent<KeyEvent>(
        [&](const KeyEvent event){
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

//     m_eventManager->subscribeEvent<MousePositionEvent>(
//         [&](MousePositionEvent event) {
// //            m_engine.submit<EntityLookParam>(
// //                    std::make_unique<EntityLook>(EntityLook{{m_boundEntityId, event.xPos, event.yPos}})
// //            );
//         }
//     );

    while (!glfwWindowShouldClose(m_window.get())) {
        glfwPollEvents();  // Process OS/window/input events
        // Optionally render your frame here
    }


}

void Client::loop() {
    while(m_shouldRun && !glfwWindowShouldClose(m_window.get())){
        glfwPollEvents();
    }
}



void DestroyGLFWWindow::operator()(GLFWwindow *ptr) const {

    glfwDestroyWindow(ptr);

}
