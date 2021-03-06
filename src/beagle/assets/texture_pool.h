//
// Created by Ricardo on 5/4/2021.
//

#ifndef BEAGLE_TEXTURE_POOL_H
#define BEAGLE_TEXTURE_POOL_H

#include <beagle/assets/asset.h>

#include <eagle/renderer/rendering_context.h>

namespace beagle {

class TextureHandle;

class TexturePool {
public:
    typedef typename std::vector<eagle::WeakPointer<eagle::Texture>>::size_type index_type;
public:
    explicit TexturePool(eagle::RenderingContext* context);

    TextureHandle insert(const std::string& filepath);
    TextureHandle insert(const std::array<std::string, 6>& filepath);
    TextureHandle insert(const eagle::TextureCreateInfo& textureCreateInfo);
    TextureHandle default_texture();

    eagle::WeakPointer<eagle::Texture>& operator[](const index_type& index) { return m_textures[index]; }

private:
    eagle::RenderingContext* m_context = nullptr;
    std::vector<eagle::WeakPointer<eagle::Texture>> m_textures;

};

class TextureHandle : public Asset<eagle::WeakPointer<eagle::Texture>, TexturePool> {
public:
    TextureHandle(TexturePool* pool, TexturePool::index_type index) : Asset<eagle::WeakPointer<eagle::Texture>, TexturePool>(pool, index) {}
};

}

#endif //BEAGLE_TEXTURE_POOL_H
