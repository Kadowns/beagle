//
// Created by Ricardo on 3/2/2022.
//

#ifndef BEAGLE_ENVIRONMENT_H
#define BEAGLE_ENVIRONMENT_H

#include <eagle/renderer/rendering_context.h>
#include <beagle/assets/texture_pool.h>

namespace beagle {

class Environment {
public:

    Environment(
                eagle::RenderingContext* context,
                const std::vector<eagle::DescriptorBindingDescription>& bindings,
                const std::vector<std::shared_ptr<eagle::Descriptor>>& descriptorItems) {
//        m_descriptorSetLayout = context->create_descriptor_set_layout(bindings, 0);
//        m_descriptorSet = context->create_descriptor_set(m_descriptorSetLayout);
    }

    std::shared_ptr<eagle::DescriptorSet> descriptor_set() { return m_descriptorSet; }

private:
    std::shared_ptr<eagle::DescriptorSetLayout> m_descriptorSetLayout;
    std::shared_ptr<eagle::DescriptorSet> m_descriptorSet;
};


class EnvironmentBuilder {
public:

    explicit EnvironmentBuilder(eagle::RenderingContext* context) : m_context(context) {

    }

    template<typename T, eagle::ShaderStage STAGE>
    EnvironmentBuilder& add_uniform_buffer(){
        eagle::DescriptorBindingDescription bindingDescription = {};
        bindingDescription.binding = m_bindings.size();
        bindingDescription.descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
        bindingDescription.size = sizeof(T);
        bindingDescription.shaderStage = STAGE;
        m_bindings.emplace_back(bindingDescription);
        m_descriptorItems.emplace_back(m_context->create_uniform_buffer(sizeof(T), nullptr));
        return *this;
    }

    template<eagle::ShaderStage STAGE>
    EnvironmentBuilder& add_texture(const TextureHandle& texture){
        eagle::DescriptorBindingDescription bindingDescription = {};
        bindingDescription.binding = m_bindings.size();
        bindingDescription.descriptorType = eagle::DescriptorType::COMBINED_IMAGE_SAMPLER;
        bindingDescription.shaderStage = STAGE;
        m_bindings.emplace_back(bindingDescription);
        m_descriptorItems.emplace_back(*texture);
        return *this;
    }

    Environment build(){
        return Environment(m_context, m_bindings, m_descriptorItems);
    }

private:
    eagle::RenderingContext* m_context;
    std::vector<eagle::DescriptorBindingDescription> m_bindings;
    std::vector<std::shared_ptr<eagle::Descriptor>> m_descriptorItems;
};

}

#endif //BEAGLE_ENVIRONMENT_H
