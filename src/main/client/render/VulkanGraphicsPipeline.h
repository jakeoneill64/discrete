#ifndef VULKANGRAPHICSPIPELINE_H
#define VULKANGRAPHICSPIPELINE_H

#include "util/Builder.h"
#include <vulkan/vulkan.h>

//
// VkGraphicsPipelineCreateInfo pipelineInfo{};

// pipelineInfo.pVertexInputState = &vertexInputInfo;
// pipelineInfo.pInputAssemblyState = &inputAssembly;
// pipelineInfo.pViewportState = &viewportState;
// pipelineInfo.pRasterizationState = &rasterizer;
// pipelineInfo.renderPass = renderPass;
// pipelineInfo.layout = pipelineLayout;
// pipelineInfo.subpass = 0;
// pipelineInfo.pDepthStencilState = &depthStencil;

class VulkanGraphicsPipeline
{
public:
    class Builder
    {
    public:
        Builder& fragmentShader(VkShaderModule fragmentShader);
        Builder& vertexShader(VkShaderModule fragmentShader);
        Builder& inputAssembly(VkPipelineInputAssemblyStateCreateInfo inputAssembly);
        Builder& rasterizer(VkPipelineRasterizationStateCreateInfo rasterization);
        Builder& viewport(VkViewport viewport);
        Builder& scissor(VkRect2D rect);
        Builder& colourBlend(VkPipelineColorBlendStateCreateInfo colourBlend);
        Builder& multisample(VkPipelineMultisampleStateCreateInfo multisample);
    };
};

#endif //VULKANGRAPHICSPIPELINE_H
