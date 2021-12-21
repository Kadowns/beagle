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

JobResult SkyboxFilterUpdateVertexUboJob::execute() {

    SkyboxFilter::VertexUbo ubo = {};
    for (auto entityId : m_dirtyCameras) {
        auto[filter, projection, transform] = m_manager->entity_from_id(entityId).components<SkyboxFilter, CameraProjection, Transform>();
        ubo.vp = projection->matrix * glm::mat4(glm::mat3(transform->inverseMatrix));
        filter->vertexShaderUbo->copy_from(&ubo, sizeof(ubo), 0);
        filter->vertexShaderUbo->upload();
    }
    m_dirtyCameras.clear();
    return JobResult::SUCCESS;
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

JobResult SkyboxFilterRenderJob::execute() {
    for (auto[camera, filter] : m_filterGroup) {

        auto commandBuffer = filter->commandBuffer;
        commandBuffer->begin(camera->passes[0].renderPass, camera->passes[0].framebuffer);
        commandBuffer->bind_vertex_buffer(filter->vertexBuffer, 0);
        commandBuffer->bind_shader(*filter->material->shader());
        commandBuffer->bind_descriptor_sets(filter->descriptorSet, 0);
        commandBuffer->bind_descriptor_sets(filter->material->descriptor_set(), 1);
        commandBuffer->draw(36);
        commandBuffer->end();
    }
    return JobResult::SUCCESS;
}

void SkyboxFilterSystem::configure(Engine* engine) {
    auto& jobs = engine->jobs();
    updateVertexUboJob = jobs.enqueue<SkyboxFilterUpdateVertexUboJob>(&engine->entities());
    renderJob = jobs.enqueue<SkyboxFilterRenderJob>(&engine->entities());
}
