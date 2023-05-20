/// 17/05/2023
/// vulkan_shader.h

#ifndef DUK_RENDERER_VULKAN_SHADER_H
#define DUK_RENDERER_VULKAN_SHADER_H

#include <duk_renderer/pipeline/shader.h>
#include <duk_renderer/pipeline/shader_data_source.h>

#include <duk_renderer/vulkan/vulkan_import.h>

namespace duk::renderer {

namespace vk {

VkShaderStageFlagBits convert_module(Shader::ModuleType::Bits module);

VkShaderStageFlags convert_module_mask(Shader::ModuleMask moduleMask);

}

struct VulkanShaderCreateInfo {
    VkDevice device;
    ShaderDataSource* shaderDataSource;
};

class VulkanShader : public Shader {
public:

    explicit VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo);

    ~VulkanShader() override;

    DUK_NO_DISCARD const std::unordered_map<Shader::ModuleType::Bits, VkShaderModule>& shader_modules() const;

    DUK_NO_DISCARD const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts() const;

    DUK_NO_DISCARD const VkPipelineLayout& pipeline_layout() const;

    DUK_NO_DISCARD hash::Hash hash() const override;

private:

    bool create_shader_module(Shader::ModuleType::Bits type, ShaderDataSource* shaderDataSource);

private:
    VkDevice m_device;
    duk::hash::Hash m_hash;
    std::unordered_map<Shader::ModuleType::Bits, VkShaderModule> m_shaderModules;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    VkPipelineLayout m_pipelineLayout;

};

}

#endif // DUK_RENDERER_VULKAN_SHADER_H

