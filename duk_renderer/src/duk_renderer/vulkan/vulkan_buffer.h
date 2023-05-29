/// 28/05/2023
/// vulkan_buffer.h

#ifndef DUK_RENDERER_VULKAN_BUFFER_H
#define DUK_RENDERER_VULKAN_BUFFER_H

#include <duk_renderer/buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_physical_device.h>
#include <duk_renderer/vulkan/command/vulkan_command_queue.h>

#include <duk_hash/hash.h>

#include <vector>

namespace duk::renderer {

struct VulkanBufferMemoryCreateInfo {
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    uint32_t queueFamilyIndex;
    VkBufferUsageFlags usageFlags;
    size_t size;
};

class VulkanBufferMemory {
protected:

    explicit VulkanBufferMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags);

public:

    virtual ~VulkanBufferMemory();

    virtual void write(void* src, size_t size, size_t offset) = 0;

    virtual void read(void* dst, size_t size, size_t offset) = 0;

    DUK_NO_DISCARD size_t size() const;

    DUK_NO_DISCARD size_t allocation_size() const;

    DUK_NO_DISCARD size_t alignment() const;

    DUK_NO_DISCARD VkBuffer handle() const;

    DUK_NO_DISCARD VkDeviceMemory memory() const;

protected:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    uint32_t m_queueFamilyIndex;
    VkBufferUsageFlags m_usageFlags;
    VkMemoryPropertyFlags m_memoryProperties;
    size_t m_size;

    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
    size_t m_allocationSize;
    size_t m_alignment;
};

class VulkanBufferHostMemory : public VulkanBufferMemory {
public:
    VulkanBufferHostMemory(const VulkanBufferMemoryCreateInfo& vulkanBufferMemoryCreateInfo);

    ~VulkanBufferHostMemory() override;

    void write(void* src, size_t size, size_t offset) override;

    void read(void* dst, size_t size, size_t offset) override;

    void map();

    void unmap();
private:
    void* m_mapped;
};

struct VulkanBufferCreateInfo {
    Buffer::Type type;
    Buffer::UpdateFrequency updateFrequency;
    size_t size;
    VkDevice device;
    VulkanPhysicalDevice* physicalDevice;
    uint32_t queueFamilyIndex;
    uint32_t imageCount;
};

class VulkanBuffer : public Buffer {
public:

    explicit VulkanBuffer(const VulkanBufferCreateInfo& bufferCreateInfo);

    ~VulkanBuffer() override;

    DUK_NO_DISCARD VkBuffer handle(uint32_t imageIndex);

    void update(uint32_t imageIndex);

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

    void read(void* dst, size_t size, size_t offset) override;

    void write(void* src, size_t size, size_t offset) override;

    DUK_NO_DISCARD size_t size() const override;

    DUK_NO_DISCARD Type type() const override;

private:
    VkDevice m_device;
    VulkanPhysicalDevice* m_physicalDevice;
    uint32_t m_queueFamilyIndex;
    Buffer::Type m_type;
    Buffer::UpdateFrequency m_updateFrequency;
    size_t m_size;
    std::vector<std::unique_ptr<VulkanBufferMemory>> m_buffers;
    std::vector<duk::hash::Hash> m_bufferHashes;
    std::vector<uint8_t> m_data;
    duk::hash::Hash m_dataHash;

};


}

#endif // DUK_RENDERER_VULKAN_BUFFER_H
