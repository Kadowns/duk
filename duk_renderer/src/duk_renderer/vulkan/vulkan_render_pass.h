/// 23/04/2023
/// vulkan_render_pass.h

#ifndef DUK_RENDERER_VULKAN_RENDER_PASS_H
#define DUK_RENDERER_VULKAN_RENDER_PASS_H

#include <duk_renderer/render_pass.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <unordered_map>
#include <memory>
#include <optional>

namespace duk::renderer {

namespace vk {

VkAttachmentStoreOp convert_store_op(StoreOp storeOp);

VkAttachmentLoadOp convert_load_op(LoadOp loadOp);

}

struct VulkanRenderPassCreateInfo {
    AttachmentDescription* colorAttachments;
    uint32_t colorAttachmentCount;
    AttachmentDescription* depthAttachment;
    VkDevice device;
};

class VulkanRenderPass : public RenderPass {
public:

    explicit VulkanRenderPass(const VulkanRenderPassCreateInfo& vulkanRenderPassCreateInfo);

    ~VulkanRenderPass() override;

    DUK_NO_DISCARD VkRenderPass handle() const;

    DUK_NO_DISCARD size_t color_attachment_count() const override;

    DUK_NO_DISCARD bool has_depth_attachment() const override;

private:
    std::vector<AttachmentDescription> m_colorAttachments;
    std::optional<AttachmentDescription> m_depthAttachment;
    VkDevice m_device;
    VkRenderPass m_renderPass;
};

}

#endif // DUK_RENDERER_VULKAN_RENDER_PASS_H
