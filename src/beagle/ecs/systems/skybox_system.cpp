//
// Created by Ricardo on 5/16/2021.
//

#include "skybox_system.h"
#include <beagle/engine.h>
#include <beagle/ecs/components/transform.h>

using namespace beagle;


SkyboxFilterUpdateVertexUboJob::SkyboxFilterUpdateVertexUboJob(EntityManager* manager) : BaseJob("SkyboxFilterUpdateVertexUboJob") {
    m_manager = manager;
    m_listener.attach(&manager->event_bus());
    m_listener.receive<OnCameraUpdate>(this);
}

void SkyboxFilterUpdateVertexUboJob::execute() {

    SkyboxFilter::VertexUbo ubo = {};
    for (auto entityId : m_dirtyCameras) {
        auto[filter, projection, transform] = m_manager->entity_from_id(entityId).components<SkyboxFilter, CameraProjection, Transform>();
        ubo.vp = projection->matrix * glm::mat4(glm::mat3(transform->inverseMatrix));
        auto cameraUbo = filter->vertexShaderUbo.lock();
        cameraUbo->copy_from(&ubo, sizeof(ubo), 0);
        cameraUbo->upload();
    }
    m_dirtyCameras.clear();

}

bool SkyboxFilterUpdateVertexUboJob::receive(const OnCameraUpdate& ev) {
    auto entity = m_manager->entity_from_id(ev.entityId);
    if (entity.has_component<SkyboxFilter>()){
        m_dirtyCameras.insert(ev.entityId);
    }
    return false;
}


SkyboxFilterRenderJob::SkyboxFilterRenderJob(EntityManager* manager) : BaseJob("SkyboxFilterRenderJob") {
    m_filterGroup.attach(manager);
}

void SkyboxFilterRenderJob::execute() {
    for (auto[camera, filter] : m_filterGroup) {

        auto commandBuffer = filter->commandBuffer.lock();
        commandBuffer->begin(camera->renderPass.lock(), camera->framebuffer.lock());
        commandBuffer->bind_vertex_buffer(filter->vertexBuffer.lock(), 0);
        commandBuffer->bind_shader(filter->material->shader()->lock());
        commandBuffer->bind_descriptor_sets(filter->descriptorSet.lock(), 0);
        commandBuffer->bind_descriptor_sets(filter->material->descriptor_set(), 1);
        commandBuffer->draw(36);
        commandBuffer->end();
    }
}

void SkyboxFilterSystem::configure(Engine* engine) {
    auto& jobs = engine->jobs();
    updateVertexUboJob = jobs.enqueue<SkyboxFilterUpdateVertexUboJob>(&engine->entities());
    renderJob = jobs.enqueue<SkyboxFilterRenderJob>(&engine->entities());
}
