/// 17/05/2023
/// vulkan_shader.cpp

#include <duk_renderer/vulkan/pipeline/vulkan_shader.h>
#include <duk_renderer/vulkan/vulkan_descriptor.h>
#include <duk_renderer/vulkan/mesh/vulkan_vertex.h>
#include <duk_renderer/vulkan/vulkan_flags.h>

#include <stdexcept>

namespace duk::renderer {

namespace vk {

VkShaderStageFlagBits convert_module(Shader::Module::Bits module) {
    VkShaderStageFlagBits stage;
    switch (module) {
        case Shader::Module::VERTEX: stage = VK_SHADER_STAGE_VERTEX_BIT; break;
        case Shader::Module::TESSELLATION_CONTROL: stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
        case Shader::Module::TESSELLATION_EVALUATION: stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
        case Shader::Module::GEOMETRY: stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
        case Shader::Module::FRAGMENT: stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
        case Shader::Module::COMPUTE: stage = VK_SHADER_STAGE_COMPUTE_BIT; break;
        default:
            throw std::runtime_error("tried to convert unsupported Shader::Module");
    }
    return stage;
}

VkShaderStageFlags convert_module_mask(Shader::Module::Mask moduleMask) {
    return convert_flags<Shader::Module>(moduleMask, convert_module);
}

}

VulkanShader::VulkanShader(const VulkanShaderCreateInfo& shaderCreateInfo) :
    m_device(shaderCreateInfo.device),
    m_hash(shaderCreateInfo.shaderDataSource->hash()),
    m_moduleMask(shaderCreateInfo.shaderDataSource->module_mask()) {

    auto shaderDataSource = shaderCreateInfo.shaderDataSource;
    if (shaderDataSource->valid_graphics_shader()) {
        Shader::Module::Bits modules[] = {
                Shader::Module::VERTEX,
                Shader::Module::TESSELLATION_CONTROL,
                Shader::Module::TESSELLATION_EVALUATION,
                Shader::Module::GEOMETRY,
                Shader::Module::FRAGMENT
                };

        for (auto module : modules) {
            create_shader_module(module, shaderDataSource);
        }

    }
    else if (shaderDataSource->valid_compute_shader()) {
        const bool created = create_shader_module(Shader::Module::COMPUTE, shaderDataSource);
        if (!created) {
            throw std::runtime_error("failed to create compute VkShaderModule");
        }
    }
    else {
        throw std::runtime_error("invalid ShaderDataSource provided to VulkanShader");
    }

    const auto& descriptorSetDescriptions = shaderDataSource->descriptor_set_descriptions();
    m_descriptorSetLayouts.reserve(descriptorSetDescriptions.size());
    for (auto& descriptorSetDescription : descriptorSetDescriptions) {

        const auto& descriptorBindings = descriptorSetDescription.bindings;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(descriptorBindings.size());
        for (auto& descriptorBinding : descriptorBindings) {
            VkDescriptorSetLayoutBinding binding = {};
            binding.binding = bindings.size();
            binding.descriptorCount = 1;
            binding.stageFlags = vk::convert_module_mask(descriptorBinding.moduleMask);
            binding.descriptorType = vk::convert_descriptor_type(descriptorBinding.type);
            bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = descriptorSetDescription.bindings.size();
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();

        VkDescriptorSetLayout descriptorSetLayout;
        auto result = vkCreateDescriptorSetLayout(m_device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create VkDescriptorSetLayout");
        }
        m_descriptorSetLayouts.push_back(descriptorSetLayout);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();

    auto result = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to create VkPipelineLayout");
    }

    auto& vertexLayout = shaderDataSource->vertex_layout();

    m_inputAttributes.reserve(vertexLayout.size());

    uint32_t attributeOffset = 0;
    for (auto& format : vertexLayout) {
        VkVertexInputAttributeDescription inputAttributeDescription = {};
        inputAttributeDescription.binding = 0;
        inputAttributeDescription.location = m_inputAttributes.size();
        inputAttributeDescription.offset = attributeOffset;
        inputAttributeDescription.format = vk::convert_vertex_attribute_format(format);

        attributeOffset += VertexAttribute::size_of(format);

        m_inputAttributes.push_back(inputAttributeDescription);
    }

    if (!m_inputAttributes.empty()) {
        VkVertexInputBindingDescription inputBindingDescription = {};
        inputBindingDescription.binding = 0;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        inputBindingDescription.stride = attributeOffset;

        m_inputBindings.push_back(inputBindingDescription);
    }
}

VulkanShader::~VulkanShader() {
    for (auto&[moduleType, module] : m_shaderModules) {
        vkDestroyShaderModule(m_device, module, nullptr);
    }
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
}

const std::vector<VkVertexInputAttributeDescription>& VulkanShader::input_attributes() const {
    return m_inputAttributes;
}

const std::vector<VkVertexInputBindingDescription>& VulkanShader::input_bindings() const {
    return m_inputBindings;
}

const std::unordered_map<Shader::Module::Bits, VkShaderModule>& VulkanShader::shader_modules() const {
    return m_shaderModules;
}

const std::vector<VkDescriptorSetLayout>& VulkanShader::descriptor_set_layouts() const {
    return m_descriptorSetLayouts;
}

const VkPipelineLayout& VulkanShader::pipeline_layout() const {
    return m_pipelineLayout;
}

hash::Hash VulkanShader::hash() const {
    return m_hash;
}

bool VulkanShader::is_graphics_shader() const {
    return m_moduleMask & (Shader::Module::VERTEX | Shader::Module::FRAGMENT);
}

bool VulkanShader::is_compute_shader() const {
    return m_moduleMask & (Shader::Module::COMPUTE);
}

bool VulkanShader::create_shader_module(Shader::Module::Bits type, ShaderDataSource* shaderDataSource) {
    if (!shaderDataSource->has_module(type)) {
        return false;
    }

    const auto& code = shaderDataSource->shader_module_spir_v_code(type);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = (const uint32_t*)code.data();

    VkShaderModule shaderModule;

    auto result = vkCreateShaderModule(m_device, &shaderModuleCreateInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create VkShaderModule");
    }

    auto[it, inserted] = m_shaderModules.emplace(type, shaderModule);
    if (!inserted) {
        throw std::runtime_error("failed to insert shader module into unordered_map");
    }

    return true;
}

}