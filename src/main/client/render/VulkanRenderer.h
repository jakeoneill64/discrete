#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <memory>
#include <vulkan/vulkan_core.h>

#include "VulkanBuffer.h"
#include "engine/world.h"
#include "client/render/Renderer.h"
#include "GLFW/glfw3.h"
#include "util/json.h"


class VulkanRendererFactory;

struct VulkanContext {
    VkInstance       instance;
    VkPhysicalDevice physicalDevice;
    VkDevice         device;
    uint32_t         graphicsQueueFamilyIndex;
    VkQueue          graphicsQueue;
    VkCommandPool    commandPool;
    VkSurfaceKHR     surface;
    VkSwapchainKHR   swapchain;
};

class VulkanRenderer final: public DiscreteRenderer
{
public:
    friend class VulkanRendererFactory;
    void operator()(std::unique_ptr<WorldView> worldView) override;
    ~VulkanRenderer() override;
    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer operator=(VulkanRenderer&) = delete;
private:

    const VulkanContext& m_context;
    VkPipeline m_graphicsPipeline;
    std::vector<VulkanBuffer> m_vertexBuffers;
    std::vector<VulkanBuffer> m_indexBuffers;
    std::vector<VulkanBuffer> m_uniformBuffers;

    explicit VulkanRenderer(const VulkanContext& context);
};

class VulkanBase{
public:
    explicit VulkanRendererFactory(
        const std::function<std::expected<JsonNode, JsonNodeError>()>& fetchConfig,
        const GLFWwindow* window
    );
    ~VulkanRendererFactory();

    // We can't copy this because it's attached to the underlying vulkan resources.
    VulkanRendererFactory(const VulkanRendererFactory&) = delete;
    VulkanRendererFactory& operator=(const VulkanRendererFactory&) = delete;

    VulkanRendererFactory(VulkanRendererFactory&&) = default;
    VulkanRendererFactory& operator=(VulkanRendererFactory&&) = default;

    const VulkanRenderer& create();

private:

    VulkanContext m_context;
    VkImage nextSwapchainImage();

};


std::vector<VulkanBuffer> m_commandBuffers;


#endif //VULKANRENDERER_H
