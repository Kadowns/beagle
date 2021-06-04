//
// Created by Ricardo on 5/4/2021.
//

#ifndef BEAGLE_SHADER_POOL_H
#define BEAGLE_SHADER_POOL_H

#include <beagle/assets/asset.h>

#include <eagle/renderer/rendering_context.h>

namespace beagle {

class ShaderHandle;

class ShaderPool {
public:
    typedef std::unordered_map<std::string, eagle::WeakPointer<eagle::Shader>> ShaderMap;
    typedef typename ShaderMap::key_type index_type;
public:
    explicit ShaderPool(eagle::RenderingContext* context);
    ShaderHandle insert(const eagle::ShaderCreateInfo& shaderCreateInfo, const index_type& name);

    eagle::WeakPointer<eagle::Shader>& operator[](const index_type& index) { return m_shaders[index]; }

private:
    eagle::RenderingContext* m_context = nullptr;
    ShaderMap m_shaders;

};

class ShaderHandle : public Asset<eagle::WeakPointer<eagle::Shader>, ShaderPool> {
public:
    ShaderHandle(ShaderPool* pool, const ShaderPool::index_type& index) : Asset<eagle::WeakPointer<eagle::Shader>, ShaderPool>(pool, index) {}
};

}

#endif //BEAGLE_SHADER_POOL_H
