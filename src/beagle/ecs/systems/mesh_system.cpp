//
// Created by Ricardo on 5/8/2021.
//
#include "mesh_system.h"
#include <beagle/engine.h>

using namespace beagle;


MeshFilterUpdateVertexUboJob::MeshFilterUpdateVertexUboJob(EntityManager* entities) {
    m_listener.attach(&entities->event_bus());
    m_listener.receive<OnCameraUpdate>(this);
    m_manager = entities;
}

bool MeshFilterUpdateVertexUboJob::receive(const OnCameraUpdate& ev) {
    m_dirtyCameras.insert(ev.entityId);
    return false;
}

JobResult MeshFilterUpdateVertexUboJob::operator()() {
    if (m_dirtyCameras.empty()){
        return JobResult::SUCCESS;
    }

    MeshFilter::VertexUbo ubo = {};
    for (auto entityId : m_dirtyCameras) {
        auto[filter, projection, transform] = m_manager->entity_from_id(entityId).components<MeshFilter, CameraProjection, Transform>();
        ubo.vp = projection->matrix * transform->inverseMatrix;
        filter->vertexShaderUbo->copy_from(&ubo, sizeof(ubo), 0);
        filter->vertexShaderUbo->upload();
    }
    m_dirtyCameras.clear();
    return JobResult::SUCCESS;
}

MeshFilterUpdateInstanceBufferJob::MeshFilterUpdateInstanceBufferJob(EntityManager* manager) {
    m_meshRendererGroup.attach(manager);
    m_meshFilterGroup.attach(manager);
}

JobResult MeshFilterUpdateInstanceBufferJob::operator()() {
    class MeshGroup {
    public:

        MeshGroup() = default;
        explicit MeshGroup(size_t capacity) :
                m_buffer((uint8_t*)malloc(capacity)),
                m_capacity(capacity),
                m_size(0){}

        MeshGroup(MeshGroup&& other)  noexcept :
                m_buffer(other.m_buffer),
                m_capacity(other.m_capacity),
                m_size(other.m_size)
        {
            other.m_buffer = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
        }

        ~MeshGroup() {
            if (m_buffer){
                free(m_buffer);
            }
        }

        void insert(void* data, size_t size) {
            memcpy(m_buffer + m_size, data, size);
            m_size += size;
        }

        void reserve(size_t capacity){
            if (m_capacity >= capacity){
                return;
            }

            m_capacity = capacity;
            if (m_buffer){
                m_buffer = (uint8_t*)realloc(m_buffer, m_capacity);
                return;
            }

            m_buffer = (uint8_t*)malloc(m_capacity);
        }


        void* data() const { return m_buffer; }
        size_t size() const { return m_size; }
        size_t capacity() const { return m_capacity; }
    private:
        uint8_t* m_buffer = nullptr;
        size_t m_capacity = 0;
        size_t m_size = 0;
    };


    class MaterialGroup {
    public:
        void insert(const MeshHandle& mesh, void* data, size_t size){
            m_meshBuffers[mesh].insert(data, size);
        }

        void reserve(const MeshHandle& mesh, size_t size){
            m_meshBuffers[mesh].reserve(size);
        }

        std::map<MeshHandle, MeshGroup>::iterator begin() { return m_meshBuffers.begin(); }
        std::map<MeshHandle, MeshGroup>::iterator end() { return m_meshBuffers.end(); }

    private:
        std::map<MeshHandle, MeshGroup> m_meshBuffers;
    };

    struct MaterialGroupCount {
        std::map<MeshHandle, uint32_t> meshGroupCount;
    };

    std::map<MaterialHandle, MaterialGroupCount> materialGroupCount;
    for (auto [transform, renderer] : m_meshRendererGroup){
        materialGroupCount[renderer->material].meshGroupCount[renderer->mesh]++;
    }


    std::map<MaterialHandle, MaterialGroup> materialGroupsData;
    for (auto[material, materialGroupCount] : materialGroupCount){
        auto& materialGroupData = materialGroupsData[material];
        for (auto[mesh, count] : materialGroupCount.meshGroupCount){
            materialGroupData.reserve(mesh, count * sizeof(glm::mat4) * 2);
        }
    }

    for (auto [transform, renderer] : m_meshRendererGroup){
        materialGroupsData[renderer->material].insert(renderer->mesh, &transform->matrix, sizeof(glm::mat4) * 2);
    }

    for (auto[filter] : m_meshFilterGroup){
        filter->materialGroups.clear();
        auto ib = filter->instanceBuffer;
        ib->clear();
        ib->reserve(m_meshRendererGroup.size() * sizeof(glm::mat4) * 2);
        size_t instanceOffset = 0;
        for (auto&[material, meshBuffers] : materialGroupsData){

            MeshFilter::MaterialGroup materialGroup = {material};
            for (auto&[mesh, buffer] : meshBuffers){
                ib->insert(buffer.data(), buffer.size());
                MeshFilter::MeshGroup meshGroup = {mesh};
                meshGroup.instanceCount = materialGroupCount[material].meshGroupCount[mesh];
                meshGroup.instanceOffset = instanceOffset;
                instanceOffset += meshGroup.instanceCount;
                materialGroup.meshGroups.emplace_back(meshGroup);
            }
            filter->materialGroups.emplace_back(materialGroup);
        }
        ib->upload();
    }
    return JobResult::SUCCESS;
}

MeshFilterUpdateFragmentUboJob::MeshFilterUpdateFragmentUboJob(EntityManager* manager) : m_manager(manager) {
    m_meshFilterGroup.attach(manager);
    m_directionalLightGroup.attach(manager);
    m_pointLightGroup.attach(manager);
}

JobResult MeshFilterUpdateFragmentUboJob::operator()() {
    MeshFilter::FragmentUbo ubo = {};

    for (auto[light, rotation] : m_directionalLightGroup){
        auto& dl = *light;
        auto& rot = *rotation;
        ubo.illumination.directionalLights[ubo.illumination.directionalLightCount].color = glm::vec4(dl.color, dl.intensity);
        ubo.illumination.directionalLights[ubo.illumination.directionalLightCount].direction = rot.quat * glm::vec3(0.0, 0.0, 1.0);
        ubo.illumination.directionalLightCount++;
        if (ubo.illumination.directionalLightCount >= MeshFilter::Illumination::maxDirectionalLights){
            break;
        }
    }

    for (auto[light, position] : m_pointLightGroup){
        auto& pl = *light;
        auto& pos = *position;

        ubo.illumination.pointLights[ubo.illumination.pointLightCount].color = glm::vec4(pl.color, pl.intensity);
        ubo.illumination.pointLights[ubo.illumination.pointLightCount].position = pos.vec;
        ubo.illumination.pointLightCount++;
        if (ubo.illumination.pointLightCount >= MeshFilter::Illumination::maxPointLights){
            break;
        }
    }

    for (auto[position, filter] : m_meshFilterGroup) {
        ubo.viewPosition = position->vec;
        auto fragmentUbo = filter->fragmentShaderUbo;
        fragmentUbo->copy_from(&ubo, sizeof(ubo), 0);
        fragmentUbo->upload();
    }
    return JobResult::SUCCESS;
}


MeshFilterRenderJob::MeshFilterRenderJob(EntityManager* manager) {
    m_meshFilterGroup.attach(manager);
}

JobResult MeshFilterRenderJob::operator()() {
    for (auto[camera, filter] : m_meshFilterGroup){
        auto commandBuffer = filter->commandBuffer;

        commandBuffer->begin(camera->passes[0].renderPass, camera->passes[0].framebuffer);

        commandBuffer->bind_index_buffer(filter->meshPool->index_buffer());
        commandBuffer->bind_vertex_buffer(filter->meshPool->vertex_buffer(), 0);
        commandBuffer->bind_vertex_buffer(filter->instanceBuffer, 1);
        for (auto& materialGroup : filter->materialGroups){
            auto& material = materialGroup.material;
            commandBuffer->bind_shader(*material->shader());
            commandBuffer->bind_descriptor_sets(filter->descriptorSet, 0);
            if (auto descriptorSet = material->descriptor_set()){
                commandBuffer->bind_descriptor_sets(descriptorSet, 1);
            }
            for (auto& meshGroup : materialGroup.meshGroups){
                commandBuffer->draw_indexed(
                        meshGroup.mesh->index_count(),
                        meshGroup.instanceCount,
                        meshGroup.mesh->first_index(),
                        meshGroup.mesh->vertex_offset(),
                        meshGroup.instanceOffset
                );
            }
        }

        commandBuffer->end();
    }
    return JobResult::SUCCESS;
}
