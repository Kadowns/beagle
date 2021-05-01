//
// Created by Ricardo on 5/1/2021.
//

#include "mesh.h"

using namespace beagle;

Mesh::Mesh(uint32_t vertexOffset, uint32_t indexOffset, uint32_t indexCount) :
    m_vertexOffset(vertexOffset), m_indexOffset(indexOffset), m_indexCount(indexCount) {}


MeshPool::MeshPool(eagle::RenderingContext* context) {
    m_vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::BAKED});
    m_indexBuffer = context->create_index_buffer({eagle::UpdateType::BAKED, eagle::IndexBufferType::UINT_32});
}

MeshHandle MeshPool::insert(void* vertices, size_t verticesSize, void* indices, size_t indicesSize, size_t indicesCount) {

    auto vb = m_vertexBuffer.lock();
    auto ib = m_indexBuffer.lock();

    vb->reserve(verticesSize);
    ib->reserve(indicesSize);

    uint32_t vertexOffset = vb->size();
    uint32_t indexOffset = ib->size();

    vb->insert(vertices, verticesSize);
    ib->insert(indices, indicesSize);

    m_meshes.emplace_back(Mesh(vertexOffset, indexOffset, indicesCount));

    return MeshHandle(this, m_meshes.size() - 1);
}

void MeshPool::upload() {
    m_vertexBuffer.lock()->upload();
    m_indexBuffer.lock()->upload();
}
