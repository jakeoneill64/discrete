#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <cstring>
#include <vulkan/vulkan.h>

struct VulkanBuffer {

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    VkDevice m_device;

    VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                 VkDeviceSize size,
                 VkBufferUsageFlags usage,
                 VkMemoryPropertyFlags properties);

    ~VulkanBuffer();
    void upload(const void* data, VkDeviceSize size) const;
};


#endif //VULKANBUFFER_H
