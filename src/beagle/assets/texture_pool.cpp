//
// Created by Ricardo on 5/4/2021.
//

#include "texture_pool.h"

using namespace beagle;

TexturePool::TexturePool(eagle::RenderingContext* context) : m_context(context) {
    eagle::TextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.filter = eagle::Filter::NEAREST;
    textureCreateInfo.imageCreateInfo.width = 2;
    textureCreateInfo.imageCreateInfo.height = 2;
    textureCreateInfo.imageCreateInfo.format = eagle::Format::R8G8B8A8_UNORM;
    textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
    textureCreateInfo.imageCreateInfo.arrayLayers = 1;
    textureCreateInfo.imageCreateInfo.mipLevels = 1;
    textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::COLOR_ATTACHMENT};
    textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    textureCreateInfo.imageCreateInfo.bufferData = {
            255, 255, 255, 255,
            255, 255, 255, 255,
            255, 255, 255, 255,
            255, 255, 255, 255
    };
    insert(textureCreateInfo);
}

TextureHandle TexturePool::insert(const eagle::TextureCreateInfo& textureCreateInfo) {
    m_textures.emplace_back(m_context->create_texture(textureCreateInfo));
    return TextureHandle(this, m_textures.size() - 1);
}

TextureHandle TexturePool::default_texture() {
    return TextureHandle(this, 0);
}
