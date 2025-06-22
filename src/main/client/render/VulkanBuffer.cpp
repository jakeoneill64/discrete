#include "VulkanBuffer.h"
#include "render.h"

VulkanBuffer::VulkanBuffer(
    const VkDevice device,
    const VkPhysicalDevice physicalDevice,
    const VkDeviceSize size,
    const VkBufferUsageFlags usage,
    const VkMemoryPropertyFlags properties
    )
: m_device(device) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, m_buffer, &memoryRequirements);

    const uint32_t memoryTypeIndex = vulkanFindMemoryIndex(
        memoryRequirements.memoryTypeBits,
        properties,
        physicalDevice);


    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, m_buffer, m_memory, 0);
}

VulkanBuffer::~VulkanBuffer()
{
    if (m_buffer != VK_NULL_HANDLE) vkDestroyBuffer(m_device, m_buffer, nullptr);
    if (m_memory != VK_NULL_HANDLE) vkFreeMemory(m_device, m_memory, nullptr);
}

void VulkanBuffer::upload(const void* data, const VkDeviceSize size) const
{
       void* mapped;
       vkMapMemory(m_device, m_memory, 0, size, 0, &mapped);
       std::memcpy(mapped, data, size);
       vkUnmapMemory(m_device, m_memory);
}


