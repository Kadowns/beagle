//
// Created by Ricardo on 5/4/2021.
//

#include <stb_image.h>
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
    eagle::TextureCreateInfo textureCreateInfo = {};
    int width, height, bpp;
    auto buffer = (uint8_t*) stbi_load(filepath.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
    if (!buffer) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }

    textureCreateInfo.imageCreateInfo.width = width;
    textureCreateInfo.imageCreateInfo.height = height;
    int len = width * height * 4;

    if (bpp == 3){
        auto tempBuffer = (uint8_t*)malloc(len);
        uint8_t* rgba = tempBuffer;
        uint8_t* rgb = buffer;
        for (size_t i = 0; i < width * height; ++i) {
            memcpy(rgba, rgb, sizeof(uint8_t) * 3);
//            memset(rgba + 3, 255, sizeof(uint8_t));
            rgba += 4;
            rgb += 3;
        }

        auto aux = buffer;
        buffer = tempBuffer;
        stbi_image_free(aux);
    }


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
