//
// Created by Ricardo on 5/8/2021.
//
#include "mesh_system.h"

using namespace beagle;


MeshFilterUpdateVertexUboJob::MeshFilterUpdateVertexUboJob(EntityManager* entities) : BaseJob("MeshFilterUpdateVertexUboJob"){
    m_listener.attach(&entities->event_bus());
    m_listener.receive<OnCameraUpdate>(this);
    m_manager = entities;
}

void MeshFilterUpdateVertexUboJob::execute() {
    if (m_dirtyCameras.empty()){
        return;
    }

    MeshFilter::VertexUbo ubo = {};
    for (auto entityId : m_dirtyCameras) {
        auto[filter, projection, transform] = m_manager->entity_from_id(entityId).components<MeshFilter, CameraProjection, Transform>();
        ubo.vp = projection->matrix * transform->inverseMatrix;
        auto cameraUbo = filter->vertexShaderUbo.lock();
        cameraUbo->copy_from(&ubo, sizeof(ubo), 0);
        cameraUbo->upload();
    }
    m_dirtyCameras.clear();
}

bool MeshFilterUpdateVertexUboJob::receive(const OnCameraUpdate& ev) {
    m_dirtyCameras.insert(ev.entityId);
    return false;
}


MeshFilterUpdateInstanceBufferJob::MeshFilterUpdateInstanceBufferJob(EntityManager* manager) : BaseJob("MeshFilterUpdateInstanceBufferJob") {
    m_meshRendererGroup.attach(manager);
    m_meshFilterGroup.attach(manager);
}

void MeshFilterUpdateInstanceBufferJob::execute() {

    class InstanceDataGroup {
    public:
        InstanceDataGroup() = default;
        explicit InstanceDataGroup(size_t capacity, MaterialHandle material) :
                m_buffer((uint8_t*)malloc(capacity)),
                m_capacity(capacity),
                m_size(0),
                m_instanceCount(0),
                m_material(material){}

        InstanceDataGroup(InstanceDataGroup&& other)  noexcept :
                m_buffer(other.m_buffer),
                m_capacity(other.m_capacity),
                m_size(other.m_size),
                m_instanceCount(other.m_instanceCount),
                m_material(other.m_material)
        {
            other.m_buffer = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_instanceCount = 0;
        }

        ~InstanceDataGroup() {
            if (m_buffer){
                free(m_buffer);
            }
        }

        void insert(void* data, size_t size) {
            memcpy(m_buffer + m_size, data, size);
            m_instanceCount++;
            m_size += size;
        }

        void* data() const { return m_buffer; }
        size_t size() const { return m_size; }
        size_t capacity() const { return m_capacity; }
        size_t instance_count() const { return m_instanceCount; }
        const MaterialHandle& material() const { return m_material; }

    private:
        uint8_t* m_buffer = nullptr;
        size_t m_capacity = 0;
        size_t m_size = 0;
        size_t m_instanceCount = 0;
        MaterialHandle m_material;
    };

    std::map<MeshHandle, InstanceDataGroup> instanceDataGroups;
    for (auto [transform, renderer] : m_meshRendererGroup){
        auto& mesh = renderer->mesh;
        if (instanceDataGroups.find(renderer->mesh) == instanceDataGroups.end()) {
            InstanceDataGroup dataGroup(m_meshRendererGroup.size() * sizeof(glm::mat4) * 2, renderer->material);
            instanceDataGroups.emplace(mesh, std::move(dataGroup));
        }
        instanceDataGroups[mesh].insert(&transform->matrix, sizeof(glm::mat4) * 2);
    }

    for (auto[filter] : m_meshFilterGroup){
        filter->meshGroups.clear();
        auto ib = filter->instanceBuffer.lock();
        ib->clear();
        ib->reserve(m_meshRendererGroup.size() * sizeof(glm::mat4) * 2);
        size_t instanceOffset = 0;
        for (auto&[mesh, instanceDataGroup] : instanceDataGroups){
            ib->insert(instanceDataGroup.data(), instanceDataGroup.size());
            MeshFilter::MeshGroup group = {mesh};
            group.material = instanceDataGroup.material();
            group.instanceCount = instanceDataGroup.instance_count();
            group.instanceOffset = instanceOffset;
            instanceOffset += instanceDataGroup.instance_count();
            filter->meshGroups.emplace_back(group);
        }
        ib->upload();
    }
}

MeshFilterUpdateFragmentUboJob::MeshFilterUpdateFragmentUboJob(EntityManager* manager) : BaseJob("MeshFilterUpdateFragmentUboJob"), m_manager(manager) {
    m_meshFilterGroup.attach(manager);
    m_directionalLightGroup.attach(manager);
    m_pointLightGroup.attach(manager);
}

void MeshFilterUpdateFragmentUboJob::execute() {

    MeshFilter::FragmentUbo ubo = {};

    for (auto[light, rotation] : m_directionalLightGroup){
        auto& dl = *light;
        auto& rot = *rotation;
        ubo.illumination.directionalLights[ubo.illumination.directionalLightCount].color = glm::vec4(dl.color, dl.intensity);
        ubo.illumination.directionalLights[ubo.illumination.directionalLightCount].direction = rot.quat * glm::vec3(0.0, 0.0, -1.0);
        ubo.illumination.directionalLightCount++;
        if (ubo.illumination.directionalLightCount >= MeshFilter::Illumination::maxDirectionalLights){
            break;
        }
    }

    for (auto[light, position] : m_pointLightGroup){
        auto& pl = *light;
        auto& pos = *position;

        ubo.illumination.pointLights[ubo.illumination.pointLightCount].color = glm::vec4(pl.color, 1.0f);
        ubo.illumination.pointLights[ubo.illumination.pointLightCount].position = pos.vec;
        ubo.illumination.pointLights[ubo.illumination.pointLightCount].constant = pl.constant;
        ubo.illumination.pointLights[ubo.illumination.pointLightCount].linear = pl.linear;
        ubo.illumination.pointLights[ubo.illumination.pointLightCount].quadratic = pl.quadratic;
        ubo.illumination.pointLightCount++;
        if (ubo.illumination.pointLightCount >= MeshFilter::Illumination::maxPointLights){
            break;
        }
    }

    for (auto[position, filter] : m_meshFilterGroup) {
        ubo.viewPosition = position->vec;
        auto fragmentUbo = filter->fragmentShaderUbo.lock();
        fragmentUbo->copy_from(&ubo, sizeof(ubo), 0);
        fragmentUbo->upload();
    }
}


MeshFilterRenderJob::MeshFilterRenderJob(EntityManager* manager) : BaseJob("MeshFilterRenderJob") {
    m_meshFilterGroup.attach(manager);
}

void MeshFilterRenderJob::execute() {

    for (auto[camera, filter] : m_meshFilterGroup){
        auto commandBuffer = filter->commandBuffer.lock();

        commandBuffer->begin(camera->renderPass.lock(), camera->framebuffer.lock());

        commandBuffer->bind_index_buffer(filter->meshPool->index_buffer());
        commandBuffer->bind_vertex_buffer(filter->meshPool->vertex_buffer(), 0);
        commandBuffer->bind_vertex_buffer(filter->instanceBuffer.lock(), 1);
        for (auto& group : filter->meshGroups){
            auto& material = group.material;
            commandBuffer->bind_shader(material->shader()->lock());
            commandBuffer->bind_descriptor_sets(filter->descriptorSet.lock(), 0);
            if (auto descriptorSet = material->descriptor_set()){
                commandBuffer->bind_descriptor_sets(descriptorSet, 1);
            }
            commandBuffer->draw_indexed(
                    group.mesh->index_count(),
                    group.instanceCount,
                    group.mesh->first_index(),
                    group.mesh->vertex_offset(),
                    group.instanceOffset
            );
        }

        commandBuffer->end();
        camera->secondaryCommandBuffers.emplace_back(commandBuffer);
    }
}