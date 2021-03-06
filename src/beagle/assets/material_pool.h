//
// Created by Ricardo on 5/3/2021.
//

#ifndef BEAGLE_MATERIAL_H
#define BEAGLE_MATERIAL_H

#include <eagle/renderer/rendering_context.h>

#include <beagle/assets/asset.h>
#include <beagle/assets/shader_pool.h>
#include <beagle/assets/texture_pool.h>

namespace beagle {

class Material {
public:

    ShaderHandle& shader() { return m_shader; }
    eagle::WeakPointer<eagle::DescriptorSet> descriptor_set() { return m_descriptorSet; }

    template<typename U>
    inline void update_uniform(size_t binding, const U& uniform) {
        update_uniform(binding, (void*)&uniform, sizeof(uniform));
    }

    template<typename U>
    inline void update_uniform(size_t binding, const std::string& name, const U& uniform) {
        update_uniform(binding, name, (void*)&uniform, sizeof(uniform));
    }

    void update_uniform(size_t binding, const std::string& name, void* data, size_t size);
    void update_uniform(size_t binding, void* data, size_t size, size_t offset = 0);
    void update_texture(size_t binding, const TextureHandle& texture);

private:
    friend class MaterialPool;
    explicit Material(eagle::RenderingContext* context, const ShaderHandle& shader, const TextureHandle& defaultTexture);

private:
    ShaderHandle m_shader;
    eagle::WeakPointer<eagle::DescriptorSet> m_descriptorSet;
    std::unordered_map<size_t, eagle::DescriptorBindingDescription> m_bindingDescriptions;
};

class MaterialHandle;

class MaterialPool {
public:
    typedef typename std::vector<Material>::size_type index_type;
public:
    explicit MaterialPool(eagle::RenderingContext* context, TexturePool* texturePool);
    MaterialHandle insert(const ShaderHandle& shader);

    Material& operator[](index_type index) { return m_materials[index]; }

private:
    eagle::RenderingContext* m_context = nullptr;
    TexturePool* m_texturePool = nullptr;
    std::vector<Material> m_materials;

};

class MaterialHandle : public Asset<Material, MaterialPool> {
public:
    MaterialHandle() : Asset<Material, MaterialPool>(nullptr, 0) {}
    MaterialHandle(MaterialPool* pool, MaterialPool::index_type index) : Asset<Material, MaterialPool>(pool, index) {}
};

}

#endif //BEAGLE_MATERIAL_H
