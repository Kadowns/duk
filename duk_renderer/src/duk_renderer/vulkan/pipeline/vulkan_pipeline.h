/// 17/05/2023
/// vulkan_pipeline.h

#ifndef DUK_RENDERER_VULKAN_PIPELINE_H
#define DUK_RENDERER_VULKAN_PIPELINE_H

#include <duk_renderer/pipeline/pipeline.h>
#include <duk_renderer/vulkan/vulkan_import.h>

#include <duk_macros/macros.h>

#include <vector>

namespace duk::renderer {

namespace vk {

VkCullModeFlagBits convert_cull_mode(Pipeline::CullMode::Bits cullModeBit);

VkCullModeFlags convert_cull_mode_mask(Pipeline::CullMode::Mask cullModeMask);

VkBlendOp convert_blend_op(Pipeline::Blend::Operator blendOperator);

VkBlendFactor convert_blend_factor(Pipeline::Blend::Factor blendFactor);

VkPrimitiveTopology convert_topology(Pipeline::Topology topology);

VkPolygonMode convert_fill_mode(Pipeline::FillMode fillMode);

}

class VulkanShader;
class VulkanRenderPass;

struct VulkanPipelineCreateInfo {
    VkDevice device;
    uint32_t imageCount;
    VulkanShader* shader;
    VulkanRenderPass* renderPass;
    Pipeline::Viewport viewport;
    Pipeline::Scissor scissor;
    Pipeline::CullMode::Mask cullModeMask;
    Pipeline::Blend blend;
    Pipeline::Topology topology;
    Pipeline::FillMode fillMode;
    bool depthTesting;
};

class VulkanPipeline : public Pipeline {
public:

    explicit VulkanPipeline(const VulkanPipelineCreateInfo& pipelineCreateInfo);

    ~VulkanPipeline() override;

    void create(uint32_t imageCount);

    void clean(uint32_t imageIndex);

    void clean();

    void update(uint32_t imageIndex);

    DUK_NO_DISCARD const VkPipeline& handle(uint32_t imageIndex) const;

    DUK_NO_DISCARD VkPipelineBindPoint bind_point() const;

    DUK_NO_DISCARD VkPipelineLayout pipeline_layout() const;

    // Pipeline overrides
    void set_viewport(const Viewport& viewport) override;

    DUK_NO_DISCARD const Viewport& viewport() const override;

    void set_scissor(const Scissor& scissor) override;

    DUK_NO_DISCARD const Scissor& scissor() const override;

    void set_blend(const Blend& blend) override;

    DUK_NO_DISCARD const Blend& blend() const override;

    void set_cull_mode(CullMode::Mask cullModeMask) override;

    DUK_NO_DISCARD CullMode::Mask cull_mode() override;

    void set_topology(Topology topology) override;

    DUK_NO_DISCARD Topology topology() const override;

    void set_fill_mode(FillMode fillMode) override;

    DUK_NO_DISCARD FillMode fill_mode() const override;

    void flush() override;

    DUK_NO_DISCARD hash::Hash hash() const override;

private:

    void update_graphics_pipeline(uint32_t imageIndex);

    void update_hash();

private:
    VkDevice m_device;
    VulkanShader* m_shader;
    VulkanRenderPass* m_renderPass;
    Pipeline::Viewport m_viewport;
    Pipeline::Scissor m_scissor;
    Pipeline::CullMode::Mask m_cullModeMask;
    Pipeline::Blend m_blend;
    Pipeline::Topology m_topology;
    Pipeline::FillMode m_fillMode;
    bool m_depthTesting;
    std::vector<VkPipeline> m_pipelines;
    VkPipelineBindPoint m_pipelineBindPoint;
    duk::hash::Hash m_hash;
    std::vector<duk::hash::Hash> m_pipelineHashes;
};

}
#endif // DUK_RENDERER_VULKAN_PIPELINE_H

