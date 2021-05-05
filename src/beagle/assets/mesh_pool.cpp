//
// Created by Ricardo on 5/1/2021.
//

#include "mesh.h"

using namespace beagle;

Mesh::Mesh(uint32_t vertexOffset, uint32_t firstIndex, uint32_t indexCount) :
        m_vertexOffset(vertexOffset), m_firstIndex(firstIndex), m_indexCount(indexCount) {}


MeshPool::MeshPool(eagle::RenderingContext* context) {
    m_vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::BAKED});
    m_indexBuffer = context->create_index_buffer({eagle::UpdateType::BAKED, eagle::IndexBufferType::UINT_32});
}

MeshHandle MeshPool::insert(void* vertices, size_t vertexCount, size_t vertexSize, void* indices, size_t indexCount,
                            size_t indexSize) {

    auto vb = m_vertexBuffer.lock();
    auto ib = m_indexBuffer.lock();

    uint32_t firstVertex = vb->size() / vertexSize;
    uint32_t firstIndex = ib->size() / indexSize;

    vb->insert(vertices, vertexCount * vertexSize);
    ib->insert(indices, indexCount * indexSize);

    m_meshes.emplace_back(Mesh(firstVertex, firstIndex, indexCount));

    return MeshHandle(this, m_meshes.size() - 1);
}

void MeshPool::upload() {
    m_vertexBuffer.lock()->upload();
    m_indexBuffer.lock()->upload();
}
