//
// Created by Ricardo on 5/3/2021.
//

#include "material_pool.h"

using namespace beagle;

Material::Material(eagle::RenderingContext* context, const ShaderHandle& shader, const TextureHandle& defaultTexture) :
    m_shader(shader) {
    auto s = m_shader->lock();
    if (s->get_descriptor_set_layouts().size() < 2){
        return;
    }
    auto descriptorSetLayout = m_shader->lock()->get_descriptor_set_layout(1).lock();
    std::vector<std::shared_ptr<eagle::DescriptorItem>> descriptors;
    m_descriptors.reserve(descriptorSetLayout->bindings().size());
    descriptors.reserve(m_descriptors.size());
    for (auto& binding : descriptorSetLayout->bindings()){
        std::weak_ptr<eagle::DescriptorItem> descriptor;
        switch(binding.descriptorType){
            case eagle::DescriptorType::UNIFORM_BUFFER:
                descriptor = context->create_uniform_buffer(binding.size, nullptr);
                break;
            case eagle::DescriptorType::STORAGE_BUFFER:
                descriptor = context->create_storage_buffer(binding.size, nullptr, eagle::UpdateType::DYNAMIC);
                break;
            case eagle::DescriptorType::SAMPLED_IMAGE:
            case eagle::DescriptorType::COMBINED_IMAGE_SAMPLER:
            case eagle::DescriptorType::STORAGE_IMAGE:
                descriptor = *defaultTexture;
                break;
        }
        m_descriptors.emplace_back(descriptor);
        descriptors.emplace_back(descriptor.lock());
    }
    m_descriptorSet = context->create_descriptor_set(descriptorSetLayout, descriptors);
}

void Material::update_uniform(size_t binding, void* data, size_t size, size_t offset) {
    if (binding >= m_descriptors.size()){
        return;
    }

    auto descriptor = m_descriptors[binding].lock();
    if (!descriptor || descriptor->type() != eagle::DescriptorType::UNIFORM_BUFFER){
        return;
    }

    auto uniformBuffer = std::static_pointer_cast<eagle::UniformBuffer>(descriptor);
    assert(uniformBuffer);
    uniformBuffer->copy_from(data, size, offset);
    uniformBuffer->upload();
}

MaterialPool::MaterialPool(eagle::RenderingContext* context, TexturePool* texturePool) :
    m_context(context), m_texturePool(texturePool) {

}

MaterialHandle MaterialPool::insert(const ShaderHandle& shader) {
    m_materials.emplace_back(Material(m_context, shader, m_texturePool->default_texture()));
    return MaterialHandle(this, m_materials.size() - 1);
}


