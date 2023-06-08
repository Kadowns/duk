/// 23/04/2023
/// vulkan_frame_buffer.h

#ifndef DUK_RENDERER_VULKAN_FRAME_BUFFER_H
#define DUK_RENDERER_VULKAN_FRAME_BUFFER_H

#include <duk_renderer/frame_buffer.h>
#include <duk_renderer/vulkan/vulkan_import.h>
#include <duk_renderer/vulkan/vulkan_image.h>

#include <vector>

namespace duk::renderer {

class VulkanSwapchain;
class VulkanRenderPass;

struct VulkanFrameBufferCreateInfo {
    uint32_t imageCount;
    VkDevice device;
    VulkanRenderPass* renderPass;
    VulkanImage** attachments;
    uint32_t attachmentCount;
};

class VulkanFrameBuffer : public FrameBuffer {
public:

    explicit VulkanFrameBuffer(const VulkanFrameBufferCreateInfo& vulkanFrameBufferCreateInfo);

    ~VulkanFrameBuffer() override;

    DUK_NO_DISCARD uint32_t width() const override;

    DUK_NO_DISCARD uint32_t height() const override;

    void create(uint32_t imageCount);

    void clean();

    void clean(uint32_t imageIndex);

    DUK_NO_DISCARD VkFramebuffer handle(uint32_t frameIndex) const;

private:
    uint32_t m_width;
    uint32_t m_height;
    VkDevice m_device;
    VulkanRenderPass* m_renderPass;
    std::vector<VulkanImage*> m_attachments;
    std::vector<VkFramebuffer> m_frameBuffers;
};


}

#endif // DUK_RENDERER_VULKAN_FRAME_BUFFER_H

