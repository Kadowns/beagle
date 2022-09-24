//
// Created by Ricardo on 5/1/2021.
//

#ifndef BEAGLE_RENDERER_H
#define BEAGLE_RENDERER_H

#include <eagle/renderer/rendering_context.h>

#include <beagle/math.h>
#include <beagle/assets/mesh_pool.h>
#include <beagle/assets/material_pool.h>

namespace beagle {

struct MeshRenderer {
    explicit MeshRenderer(const MeshHandle& mesh, const MaterialHandle& material) :
    mesh(mesh), material(material){}
    MeshHandle mesh;
    MaterialHandle material;
};

struct MeshFilter {

    struct MeshGroup {
        MeshHandle mesh;
        uint32_t instanceCount;
        uint32_t instanceOffset;
    };

    struct MaterialGroup {
        MaterialHandle material;
        std::vector<MeshGroup> meshGroups;
    };

    struct VertexUbo {
        glm::mat4 vp;
    };

    struct DirectionalLight {
        alignas(16) glm::vec4 color;
        alignas(16) glm::vec3 direction;
    };

    struct PointLight {
        glm::vec4 color;
        alignas(16) glm::vec3 position;
    };

    struct Illumination {
        static const uint32_t maxDirectionalLights = 4;
        static const uint32_t maxPointLights = 16;
        DirectionalLight directionalLights[maxDirectionalLights];
        uint32_t directionalLightCount = 0;
        PointLight pointLights[maxPointLights];
        uint32_t pointLightCount = 0;
    };

    struct FragmentUbo {
        Illumination illumination;
        glm::vec3 viewPosition;
    };


    explicit MeshFilter(eagle::RenderingContext* context, MeshPool* meshPool, const TextureHandle& irradianceMap,
                        const TextureHandle& prefilteredMap, const TextureHandle& brdfLut) : meshPool(meshPool) {
        commandBuffer = context->create_command_buffer({eagle::CommandBufferLevel::SECONDARY});
        instanceBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});
        vertexShaderUbo = context->create_uniform_buffer(sizeof(VertexUbo), nullptr);
        fragmentShaderUbo = context->create_uniform_buffer(sizeof(FragmentUbo), nullptr);

        std::vector<eagle::DescriptorBindingDescription> bindings;
        bindings.resize(5);
        bindings[0].binding = 0;
        bindings[0].descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
        bindings[0].size = sizeof(VertexUbo);
        bindings[0].shaderStage = eagle::ShaderStage::VERTEX;

        bindings[1].binding = 1;
        bindings[1].descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
        bindings[1].size = sizeof(FragmentUbo);
        bindings[1].shaderStage = eagle::ShaderStage::FRAGMENT;

        //irradiance map
        bindings[2].binding = 2;
        bindings[2].descriptorType = eagle::DescriptorType::COMBINED_IMAGE_SAMPLER;
        bindings[2].shaderStage = eagle::ShaderStage::FRAGMENT;

        //prefiltered map
        bindings[3].binding = 3;
        bindings[3].descriptorType = eagle::DescriptorType::COMBINED_IMAGE_SAMPLER;
        bindings[3].shaderStage = eagle::ShaderStage::FRAGMENT;

        //brdf lut
        bindings[4].binding = 4;
        bindings[4].descriptorType = eagle::DescriptorType::COMBINED_IMAGE_SAMPLER;
        bindings[4].shaderStage = eagle::ShaderStage::FRAGMENT;


        descriptorSetLayout = context->create_descriptor_set_layout(bindings, 0);
        descriptorSet = context->create_descriptor_set(descriptorSetLayout);
    }

    MeshPool* meshPool;
    std::shared_ptr<eagle::CommandBuffer> commandBuffer;
    std::shared_ptr<eagle::DescriptorSet> descriptorSet;
    std::shared_ptr<eagle::DescriptorSetLayout> descriptorSetLayout;
    std::shared_ptr<eagle::UniformBuffer> vertexShaderUbo, fragmentShaderUbo;
    std::vector<MaterialGroup> materialGroups;
};

}

#endif //BEAGLE_RENDERER_H
