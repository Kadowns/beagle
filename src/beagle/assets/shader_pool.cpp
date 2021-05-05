//
// Created by Ricardo on 5/4/2021.
//

#include "shader_pool.h"

beagle::ShaderPool::ShaderPool(eagle::RenderingContext* context) : m_context(context) {

}

beagle::ShaderHandle beagle::ShaderPool::insert(const eagle::ShaderCreateInfo& shaderCreateInfo,
                                                const beagle::ShaderPool::index_type& name) {
    m_shaders.emplace(name, m_context->create_shader(shaderCreateInfo));
    return ShaderHandle(this, name);
}
