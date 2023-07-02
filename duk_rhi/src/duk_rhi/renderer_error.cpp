//
// Created by Ricardo on 09/04/2023.
//

#include <duk_rhi/renderer_error.h>

namespace duk::rhi {


RendererError::RendererError(RendererError::Type type, const std::string& description) :
    m_type(type),
    m_description(description) {

}

RendererError::Type RendererError::type() const {
    return m_type;
}

const std::string& RendererError::description() const {
    return m_description;
}
}
