//
// Created by Ricardo on 5/3/2021.
//

#include "material_pool.h"

using namespace beagle;

Material::Material(eagle::RenderingContext* context, const ShaderHandle& shader, const TextureHandle& defaultTexture) :
    m_shader(shader) {
    auto s = m_shader->get();
    if (s->descriptor_set_layouts().size() < 2){
        return;
    }
    auto descriptorSetLayout = s->descriptor_set_layout(1);
    std::vector<std::shared_ptr<eagle::Descriptor>> descriptors;
    descriptors.reserve(descriptorSetLayout->info().bindings.size());
    for (auto& binding : descriptorSetLayout->info().bindings){
        std::shared_ptr<eagle::Descriptor> descriptor;
        switch(binding.descriptorType){
            case eagle::DescriptorType::UNIFORM_BUFFER:
                descriptor = context->create_uniform_buffer(binding.size, nullptr);
                break;
            case eagle::DescriptorType::STORAGE_BUFFER:
                descriptor = context->create_storage_buffer(binding.size, nullptr, eagle::UpdateType::DYNAMIC);
                break;
            case eagle::DescriptorType::COMBINED_IMAGE_SAMPLER:
                descriptor = (*defaultTexture);
                break;
            case eagle::DescriptorType::SAMPLED_IMAGE:
            case eagle::DescriptorType::STORAGE_IMAGE:
                descriptor = (*defaultTexture)->image()->view();
                break;
        }
        m_bindingDescriptions.emplace(binding.binding, binding);
        descriptors.emplace_back(descriptor);
    }

    eagle::DescriptorSetInfo descriptorSetInfo = {};
    descriptorSetInfo.layout = descriptorSetLayout;
    descriptorSetInfo.descriptors = descriptors;
    m_descriptorSet = context->create_descriptor_set(descriptorSetInfo);
}

void Material::update_uniform(size_t binding, void* data, size_t size, size_t offset) {
    auto descriptorSet = m_descriptorSet;
    if (binding >= descriptorSet->size()){
        return;
    }

    auto descriptor = descriptorSet->operator[](binding);
    if (!descriptor || descriptor->type() != eagle::DescriptorType::UNIFORM_BUFFER){
        return;
    }


    auto uniformBuffer = std::static_pointer_cast<eagle::UniformBuffer>(descriptor);
    assert(uniformBuffer);
    uniformBuffer->copy_from(data, size, offset);
    uniformBuffer->upload();
}

void Material::update_texture(size_t binding, const TextureHandle& texture) {
    auto descriptorSet = m_descriptorSet;
    if (binding >= descriptorSet->size()){
        return;
    }
    descriptorSet->operator[](binding) = *texture;
    m_descriptorSet->update();
}

void Material::update_uniform(size_t binding, const std::string& name, void* data, size_t size) {
    auto descriptorSet = m_descriptorSet;
    if (binding >= descriptorSet->size()){
        return;
    }

    auto descriptor = descriptorSet->operator[](binding);
    if (!descriptor || descriptor->type() != eagle::DescriptorType::UNIFORM_BUFFER){
        return;
    }

    auto& bindingDescription = m_bindingDescriptions[binding];

    auto it = bindingDescription.members.find(name);
    if (it == bindingDescription.members.end()){
        return;
    }

    auto& member = it->second;

    auto uniformBuffer = std::static_pointer_cast<eagle::UniformBuffer>(descriptor);
    assert(uniformBuffer);
    uniformBuffer->copy_from(data, size, member.offset);
    uniformBuffer->upload();
}

MaterialPool::MaterialPool(eagle::RenderingContext* context, TexturePool* texturePool) :
    m_context(context), m_texturePool(texturePool) {

}

MaterialHandle MaterialPool::insert(const ShaderHandle& shader) {
    m_materials.emplace_back(Material(m_context, shader, m_texturePool->default_texture()));
    return MaterialHandle(this, m_materials.size() - 1);
}


