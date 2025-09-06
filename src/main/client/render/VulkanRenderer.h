#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <memory>
#include <vulkan/vulkan_core.h>

#include "engine/world.h"
#include "client/render/Renderer.h"
#include "util/json.h"


class VulkanRenderer final: public DiscreteRenderer
{

    public:
    void operator()(std::unique_ptr<WorldView> worldView) override;
    ~VulkanRenderer() override;

    private:
    VulkanRenderer();

};

class PartialVulkanRenderer{
public:
    explicit PartialVulkanRenderer(const std::function<std::expected<JsonNode, JsonNodeError>()>& fetchConfig);
    ~PartialVulkanRenderer(); // TODO cleanup vk resources.
    PartialVulkanRenderer(const PartialVulkanRenderer&) = delete;
    PartialVulkanRenderer& operator()();
private:
    VkInstance                m_instance;
    VkPhysicalDevice          m_physicalDevice;
    VkDevice                  m_device;
    uint32_t                  m_graphicsQueueFamilyIndex;
    VkQueue                   m_graphicsQueue;
    VkCommandPool             m_commandPool;
};

#endif //VULKANRENDERER_H
