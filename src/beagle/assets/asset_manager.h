//
// Created by Ricardo on 5/4/2021.
//

#ifndef BEAGLE_ASSET_MANAGER_H
#define BEAGLE_ASSET_MANAGER_H

#include <beagle/assets/texture_pool.h>
#include <beagle/assets/shader_pool.h>
#include <beagle/assets/mesh_pool.h>
#include <beagle/assets/material_pool.h>

namespace beagle {

class AssetManager {
public:

    explicit AssetManager(eagle::RenderingContext* context);

    inline TexturePool& texture_pool() { return m_texturePool; }
    inline ShaderPool& shader_pool() { return m_shaderPool; }
    inline MaterialPool& material_pool() { return m_materialPool; }
    inline MeshPool& mesh_pool() { return m_meshPool; }

private:
    TexturePool m_texturePool;
    ShaderPool m_shaderPool;
    MaterialPool m_materialPool;
    MeshPool m_meshPool;
};

}

#endif //TEMPLATE_APP_ASSET_MANAGER_H
