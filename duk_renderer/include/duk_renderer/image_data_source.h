/// 04/06/2023
/// image_data_source.h

#ifndef DUK_RENDERER_IMAGE_DATA_SOURCE_H
#define DUK_RENDERER_IMAGE_DATA_SOURCE_H

#include <duk_renderer/data_source.h>
#include <duk_renderer/image.h>
#include <cstring>

namespace duk::renderer {

class ImageDataSource : public DataSource {
public:

    DUK_NO_DISCARD virtual Image::PixelFormat pixel_format() const = 0;

    DUK_NO_DISCARD virtual size_t pixel_count() const = 0;

    DUK_NO_DISCARD virtual size_t byte_count() const = 0;

    DUK_NO_DISCARD virtual size_t width() const = 0;

    DUK_NO_DISCARD virtual size_t height() const = 0;

    virtual void read_bytes(void* dst, size_t size, size_t offset) const = 0;

    template<typename PixelType>
    void read_pixels(PixelType* dst, size_t pixelCount, size_t pixelOffset) {
        const auto pixelByteSize = Image::pixel_format_size(pixel_format());
        assert(pixelByteSize == sizeof(PixelType));
        assert(pixel_count() >= pixelCount + pixelOffset);
        read_bytes((void*)dst, pixelCount * pixelByteSize, pixelOffset * pixelByteSize);
    }
};

template<typename PixelType, Image::PixelFormat PixelFormat>
class StdImageDataSource : public ImageDataSource {
public:
    StdImageDataSource(uint32_t width, uint32_t height) :
        m_width(width),
        m_height(height),
        m_pixels(width * height) {

    }

    void write_pixel(uint32_t x, uint32_t y, const PixelType& pixel) {
        assert(x < m_width);
        assert(y < m_height);
        m_pixels[x + (y * m_width)] = pixel;
    }

    void resize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
        m_pixels.resize(m_width * m_height);
    }

    DUK_NO_DISCARD Image::PixelFormat pixel_format() const override {
        return PixelFormat;
    }

    DUK_NO_DISCARD size_t pixel_count() const override {
        return m_pixels.size();
    }

    DUK_NO_DISCARD size_t byte_count() const override {
        return m_pixels.size() * sizeof(PixelType);
    }

    DUK_NO_DISCARD size_t width() const override {
        return m_width;
    }

    DUK_NO_DISCARD size_t height() const override {
        return m_height;
    }

    void read_bytes(void* dst, size_t size, size_t offset) const override {
        assert(byte_count() >= size + offset);
        const uint8_t* src = (const uint8_t*)m_pixels.data();
        memcpy(dst, (const void*)(src + offset), size);
    }

protected:

    DUK_NO_DISCARD duk::hash::Hash calculate_hash() const override {
        duk::hash::Hash hash = 0;
        duk::hash::hash_combine(hash, m_pixels.data(), byte_count());
        return hash;
    }

private:
    uint32_t m_width;
    uint32_t m_height;
    std::vector<PixelType> m_pixels;
};

}

#endif // DUK_RENDERER_IMAGE_DATA_SOURCE_H

