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


    explicit MeshFilter(eagle::RenderingContext* context, MeshPool* meshPool) : meshPool(meshPool) {
        commandBuffer = context->create_command_buffer({eagle::CommandBufferLevel::SECONDARY});
        instanceBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});
        vertexShaderUbo = context->create_uniform_buffer(sizeof(VertexUbo), nullptr);
        fragmentShaderUbo = context->create_uniform_buffer(sizeof(FragmentUbo), nullptr);

        eagle::DescriptorBindingDescription vertBinding = {};
        vertBinding.binding = 0;
        vertBinding.descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
        vertBinding.size = sizeof(VertexUbo);
        vertBinding.shaderStage = eagle::ShaderStage::VERTEX;

        eagle::DescriptorBindingDescription fragBinding = {};
        fragBinding.binding = 1;
        fragBinding.descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
        fragBinding.size = sizeof(FragmentUbo);
        fragBinding.shaderStage = eagle::ShaderStage::FRAGMENT;

        descriptorSetLayout = context->create_descriptor_set_layout({vertBinding, fragBinding});
        descriptorSet = context->create_descriptor_set(descriptorSetLayout, {vertexShaderUbo, fragmentShaderUbo});
    }

    MeshPool* meshPool;
    eagle::WeakPointer<eagle::CommandBuffer> commandBuffer;
    eagle::WeakPointer<eagle::VertexBuffer> instanceBuffer;
    eagle::WeakPointer<eagle::DescriptorSet> descriptorSet;
    eagle::WeakPointer<eagle::DescriptorSetLayout> descriptorSetLayout;
    eagle::WeakPointer<eagle::UniformBuffer> vertexShaderUbo, fragmentShaderUbo;
    std::vector<MaterialGroup> materialGroups;
};

}

#endif //BEAGLE_RENDERER_H
