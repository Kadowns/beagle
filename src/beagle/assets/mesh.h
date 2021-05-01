//
// Created by Ricardo on 5/1/2021.
//

#ifndef BEAGLE_MESH_H
#define BEAGLE_MESH_H

#include <beagle/assets/asset.h>

#include <eagle/renderer/rendering_context.h>

namespace beagle {

class Mesh {
public:

    inline uint32_t vertex_offset() const { return m_vertexOffset; }
    inline uint32_t index_offset() const { return m_indexOffset; }
    inline uint32_t index_count() const { return m_indexCount; }

private:
    friend class MeshPool;
    Mesh(uint32_t vertexOffset, uint32_t indexOffset, uint32_t indexCount);

private:
    uint32_t m_vertexOffset;
    uint32_t m_indexOffset;
    uint32_t m_indexCount;
};

class MeshHandle;

class MeshPool {
public:
    typedef typename std::vector<Mesh>::size_type size_type;

public:

    explicit MeshPool(eagle::RenderingContext* context);

    template<typename V, typename I>
    MeshHandle insert(const std::vector<V>& vertices, const std::vector<I>& indices);
    MeshHandle insert(void* vertices, size_t verticesSize, void* indices, size_t indicesSize, size_t indicesCount);
    void upload();

    Mesh& operator[](size_type index) { return m_meshes[index]; }

    inline std::shared_ptr<eagle::VertexBuffer> vertex_buffer() const { return m_vertexBuffer.lock(); }
    inline std::shared_ptr<eagle::IndexBuffer> index_buffer() const { return m_indexBuffer.lock(); }

private:
    std::vector<Mesh> m_meshes;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
};

class MeshHandle : public Asset<Mesh, MeshPool> {
public:
    MeshHandle(MeshPool* pool, MeshPool::size_type index) : Asset<Mesh, MeshPool>(pool, index) {}
};

template<typename V, typename I>
MeshHandle MeshPool::insert(const std::vector<V>& vertices, const std::vector<I>& indices){
    static_assert(sizeof(I) == sizeof(uint32_t), "Invalid mesh index size");//TODO allow different index sizes
    return insert(vertices.data(), vertices.size() * sizeof(V), indices.data(), indices.size() * sizeof(I), indices.size());
}

}

#endif //BEAGLE_MESH_H
