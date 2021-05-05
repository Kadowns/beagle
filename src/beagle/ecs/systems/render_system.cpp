//
// Created by Ricardo on 4/30/2021.
//

#include "render_system.h"

#include <utility>

using namespace beagle;

RenderBeginJob::RenderBeginJob(eagle::RenderingContext* context) : m_context(context), BaseJob("RenderBeginJob") {

}

void RenderBeginJob::execute() {
    if (!m_context->prepare_frame()){
        return;
    }
}

RenderEndJob::RenderEndJob(eagle::RenderingContext* context) : m_context(context), BaseJob("RenderEndJob") {

}

void RenderEndJob::execute() {
    m_context->present_frame();
}

BuildMeshGroupsJob::BuildMeshGroupsJob(EntityManager* manager) : BaseJob("BuildMeshGroupsJob") {
    m_meshRendererGroup.attach(manager);
    m_meshFilterGroup.attach(manager);
}

void BuildMeshGroupsJob::execute() {

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
            InstanceDataGroup dataGroup(m_meshRendererGroup.size() * sizeof(glm::mat4), renderer->material);
            instanceDataGroups.emplace(mesh, std::move(dataGroup));
        }
        instanceDataGroups[mesh].insert(&transform->matrix, sizeof(glm::mat4));
    }

    for (auto[filter] : m_meshFilterGroup){
        filter->meshGroups.clear();
        auto ib = filter->instanceBuffer.lock();
        ib->clear();
        ib->reserve(m_meshRendererGroup.size() * sizeof(glm::mat4));
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

RenderMeshFilterJob::RenderMeshFilterJob(EntityManager* manager) : BaseJob("RenderMeshFilterJob") {
    m_meshFilterGroup.attach(manager);
}

void RenderMeshFilterJob::execute() {

    for (auto[camera, filter] : m_meshFilterGroup){
        auto commandBuffer = filter->commandBuffer.lock();

        commandBuffer->begin(camera->renderPass.lock(), camera->framebuffer.lock());

        commandBuffer->bind_index_buffer(filter->meshPool->index_buffer());
        commandBuffer->bind_vertex_buffer(filter->meshPool->vertex_buffer(), 0);
        commandBuffer->bind_vertex_buffer(filter->instanceBuffer.lock(), 1);
        for (auto& group : filter->meshGroups){
            auto& material = group.material;
            commandBuffer->bind_shader(material->shader()->lock());
            commandBuffer->bind_descriptor_sets(camera->descriptorSet.lock(), 0);
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

RenderCameraJob::RenderCameraJob(EntityManager* manager) : BaseJob("RenderCameraJob") {
    m_cameraGroup.attach(manager);
}

void RenderCameraJob::execute() {

    for (auto[camera] : m_cameraGroup){

        auto commandBuffer = camera->commandBuffer.lock();
        commandBuffer->begin();
        commandBuffer->begin_render_pass(camera->renderPass.lock(), camera->framebuffer.lock());
        commandBuffer->execute_commands(camera->secondaryCommandBuffers);
        commandBuffer->end_render_pass();
        commandBuffer->end();
        camera->context->submit_command_buffer(commandBuffer);
        camera->secondaryCommandBuffers.clear();
    }
}
