//
// Created by Ricardo on 5/1/2021.
//

#ifndef BEAGLE_RENDERER_H
#define BEAGLE_RENDERER_H

#include <eagle/renderer/rendering_context.h>

#include <beagle/assets/mesh.h>

namespace beagle {

struct MeshRenderer {
    explicit MeshRenderer(const MeshHandle& mesh, const std::shared_ptr<eagle::Shader>& shader) :
    mesh(mesh), shader(shader){}
    MeshHandle mesh;
    std::weak_ptr<eagle::Shader> shader;
};

struct MeshFilter {

    explicit MeshFilter(eagle::RenderingContext* context, MeshPool* meshPool) : meshPool(meshPool) {
        commandBuffer = context->create_command_buffer({eagle::CommandBufferLevel::SECONDARY});
        instanceBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});
    }

    struct MeshGroup {
        MeshHandle mesh;
        std::weak_ptr<eagle::Shader> shader;
        uint32_t instanceCount;
        uint32_t instanceOffset;
    };

    MeshPool* meshPool;
    std::weak_ptr<eagle::CommandBuffer> commandBuffer;
    std::weak_ptr<eagle::VertexBuffer> instanceBuffer;
    std::vector<MeshGroup> meshGroups;
};

}

#endif //BEAGLE_RENDERER_H
