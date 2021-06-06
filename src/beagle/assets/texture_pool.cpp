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
    textureCreateInfo.imageCreateInfo.buffer = {
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


    if (stbi_is_hdr_from_memory(bytes.data(), bytes.size())){
        float* buffer = stbi_loadf_from_memory(bytes.data(), bytes.size(), &width, &height, &bpp, STBI_rgb_alpha);
        if (!buffer) {
            throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
        }

        textureCreateInfo.imageCreateInfo.width = width;
        textureCreateInfo.imageCreateInfo.height = height;
        size_t len = width * height * 4 * sizeof(float);

        textureCreateInfo.imageCreateInfo.buffer.resize(len);
        memcpy(textureCreateInfo.imageCreateInfo.buffer.data(), buffer, len);
        stbi_image_free(buffer);

        textureCreateInfo.imageCreateInfo.format = eagle::Format::R32G32B32A32_SFLOAT;
        textureCreateInfo.imageCreateInfo.mipLevels = 1;//static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;;
        textureCreateInfo.imageCreateInfo.arrayLayers = 1;
        textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
        textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
        textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
        textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_SRC, eagle::ImageUsage::TRANSFER_DST};
        textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        textureCreateInfo.filter = eagle::Filter::LINEAR;
    }
    else {
        uint8_t* buffer = stbi_load_from_memory(bytes.data(), bytes.size(), &width, &height, &bpp, STBI_rgb_alpha);
        if (!buffer) {
            throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
        }

        textureCreateInfo.imageCreateInfo.width = width;
        textureCreateInfo.imageCreateInfo.height = height;
        int len = width * height * 4;

        textureCreateInfo.imageCreateInfo.buffer.resize(len);
        memcpy(textureCreateInfo.imageCreateInfo.buffer.data(), buffer, len);
        stbi_image_free(buffer);

        textureCreateInfo.imageCreateInfo.format = eagle::Format::R8G8B8A8_UNORM;
        textureCreateInfo.imageCreateInfo.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
        textureCreateInfo.imageCreateInfo.arrayLayers = 1;
        textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
        textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
        textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
        textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_SRC, eagle::ImageUsage::TRANSFER_DST};
        textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        textureCreateInfo.filter = eagle::Filter::LINEAR;
    }
    auto texture = insert(textureCreateInfo);
    texture->get()->image()->generate_mipmaps();
    return texture;
}

TextureHandle TexturePool::insert(const std::array<std::string, 6>& filepaths) {

    eagle::TextureCreateInfo textureCreateInfo = {};

    for (size_t i = 0; i < filepaths.size(); i++){
        auto bytes = eagle::FileSystem::instance()->read_bytes(filepaths[i]);
        int width, height, bpp;
        auto data = (uint8_t*) stbi_load_from_memory(bytes.data(), bytes.size(), &width, &height, &bpp, STBI_rgb_alpha);

        if (textureCreateInfo.imageCreateInfo.buffer.empty()){
            textureCreateInfo.imageCreateInfo.width = width;
            textureCreateInfo.imageCreateInfo.height = height;
            textureCreateInfo.imageCreateInfo.buffer.resize(width * height * 24);//reserve enough memory for all 6 faces
        }
        assert(textureCreateInfo.imageCreateInfo.width == width && "Cube face with unequal width");
        assert(textureCreateInfo.imageCreateInfo.height == height && "Cube face with unequal height");

        size_t size = width * height * 4;
        memcpy(textureCreateInfo.imageCreateInfo.buffer.data() + i * size, data, size);
        stbi_image_free(data);
    }

    textureCreateInfo.imageCreateInfo.format = eagle::Format::R8G8B8A8_UNORM;
    textureCreateInfo.imageCreateInfo.mipLevels = 1;
    textureCreateInfo.imageCreateInfo.arrayLayers = 6;
    textureCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    textureCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    textureCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    textureCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_DST};
    textureCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
    textureCreateInfo.imageCreateInfo.type = eagle::ImageType::CUBE;
    textureCreateInfo.filter = eagle::Filter::LINEAR;
    return insert(textureCreateInfo);
}
