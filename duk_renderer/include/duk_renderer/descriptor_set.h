/// 31/05/2023
/// descriptor_set.h

#ifndef DUK_RENDERER_DESCRIPTOR_SET_H
#define DUK_RENDERER_DESCRIPTOR_SET_H

#include <duk_renderer/descriptor.h>
#include <duk_macros/macros.h>

namespace duk::renderer {

class Image;
class Buffer;

class DescriptorSet {
public:

    virtual ~DescriptorSet();

    DUK_NO_DISCARD virtual Descriptor& at(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Descriptor& at(uint32_t binding) const = 0;

    DUK_NO_DISCARD virtual Image* image(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Image* image(uint32_t binding) const = 0;

    DUK_NO_DISCARD virtual Buffer* buffer(uint32_t binding) = 0;

    DUK_NO_DISCARD virtual const Buffer* buffer(uint32_t binding) const = 0;

    virtual void flush() = 0;

};

}

#endif // DUK_RENDERER_DESCRIPTOR_SET_H

