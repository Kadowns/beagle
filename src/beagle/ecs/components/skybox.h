//
// Created by Ricardo on 5/16/2021.
//

#ifndef BEAGLE_SKYBOX_H
#define BEAGLE_SKYBOX_H

#include <beagle/assets/material_pool.h>

namespace beagle {

struct SkyboxFilter {

    struct VertexUbo {
        glm::mat4 vp;
    };

    explicit SkyboxFilter(eagle::RenderingContext* context, MaterialHandle material) : material(material) {
        float skyboxVertices[] = {
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };

        vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::BAKED, sizeof(skyboxVertices), skyboxVertices});
        vertexShaderUbo = context->create_uniform_buffer(sizeof(VertexUbo), nullptr);
        descriptorSet = context->create_descriptor_set((*material->shader())->get_descriptor_set_layout(0), {vertexShaderUbo});
        commandBuffer = context->create_command_buffer({eagle::CommandBufferLevel::SECONDARY});
    }


    MaterialHandle material;
    eagle::WeakPointer<eagle::VertexBuffer> vertexBuffer;
    eagle::WeakPointer<eagle::UniformBuffer> vertexShaderUbo;
    eagle::WeakPointer<eagle::DescriptorSet> descriptorSet;
    eagle::WeakPointer<eagle::CommandBuffer> commandBuffer;
};

}

#endif //BEAGLE_SKYBOX_H
