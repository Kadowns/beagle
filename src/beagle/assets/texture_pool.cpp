//
// Created by Ricardo on 5/4/2021.
//

#include <stb_image.h>
#include <eagle/file_system.h>
#include "texture_pool.h"

using namespace beagle;

TexturePool::TexturePool(eagle::RenderingContext* context) : m_context(context) {
    eagle::TextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.filter = eagle::Filter::NEAREST;
    textureCreateInfo.imageCreateInfo.width = 1;
    textureCreateInfo.imageCreateInfo.height = 1;
    textureCreateInfo.imageCreateInfo.format = eagle::Format::R8G8B8A8_UNORM;
    textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
    textureCreateInfo.imageCreateInfo.arrayLayers = 1;
    textureCreateInfo.imageCreateInfo.mipLevels = 1;
    textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_DST};
    textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    textureCreateInfo.imageCreateInfo.bufferData = {
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

TextureHandle TexturePool::insert(const std::string& filepath) {

    auto bytes = eagle::FileSystem::instance()->read_bytes(filepath);

    eagle::TextureCreateInfo textureCreateInfo = {};
    int width, height, bpp;

    auto buffer = (uint8_t*) stbi_load_from_memory(bytes.data(), bytes.size(), &width, &height, &bpp, STBI_rgb_alpha);
    if (!buffer) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }

    textureCreateInfo.imageCreateInfo.width = width;
    textureCreateInfo.imageCreateInfo.height = height;
    int len = width * height * 4;

    textureCreateInfo.imageCreateInfo.bufferData.resize(len);
    memcpy(textureCreateInfo.imageCreateInfo.bufferData.data(), buffer, len);
    stbi_image_free(buffer);

    textureCreateInfo.imageCreateInfo.format = eagle::Format::R8G8B8A8_UNORM;
    textureCreateInfo.imageCreateInfo.mipLevels = 1;
    textureCreateInfo.imageCreateInfo.arrayLayers = 1;
    textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::LINEAR;
    textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_DST};
    textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
    textureCreateInfo.filter = eagle::Filter::LINEAR;
    return insert(textureCreateInfo);
}
