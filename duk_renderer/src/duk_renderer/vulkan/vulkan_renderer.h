//
// Created by Ricardo on 04/02/2023.
//

#ifndef DUK_RENDERER_VULKAN_RENDERER_H
#define DUK_RENDERER_VULKAN_RENDERER_H

#include <duk_renderer/renderer.h>

namespace duk::renderer {

struct VulkanRendererCreateInfo {

};

class VulkanRenderer : public Renderer {
public:

    VulkanRenderer(const VulkanRendererCreateInfo& vulkanRendererCreateInfo);

    void begin_frame() override;

    void end_frame() override;

private:

};

}

#endif //DUK_RENDERER_VULKAN_RENDERER_H
