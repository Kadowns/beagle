//
// Created by Ricardo on 4/25/2021.
//

#include "camera_system.h"
#include <beagle/engine.h>

using namespace beagle;

CameraUpdateOrthographicProjectionJob::CameraUpdateOrthographicProjectionJob(EntityManager* entities, float width, float height) :
    BaseJob("CameraUpdateOrthographicProjectionJob") {
    m_eventBus = &entities->event_bus();
    m_width = width;
    m_height = height;
    m_listener.attach(m_eventBus);
    m_listener.receive<eagle::OnWindowResized>(this);
    m_cameraGroup.attach(entities);
}

JobResult CameraUpdateOrthographicProjectionJob::execute() {
    if (!m_windowResized){
        return JobResult::SUCCESS;
    }
    m_windowResized = false;

    for (auto[projection, orthographic] : m_cameraGroup){
        orthographic->left = 0.0f;
        orthographic->right = m_width;
        orthographic->top = 0.0f;
        orthographic->bottom = m_height;

        projection->matrix = glm::ortho(
                orthographic->left,
                orthographic->right,
                orthographic->bottom,
                orthographic->top,
                orthographic->near,
                orthographic->far
                );

        m_eventBus->emit(OnCameraUpdate{projection.owner().id()});
    }
    return JobResult::SUCCESS;
}

bool CameraUpdateOrthographicProjectionJob::receive(const eagle::OnWindowResized& ev) {
    m_windowResized = true;
    m_width = ev.width;
    m_height = ev.height;
    return false;
}

CameraUpdatePerspectiveProjectionJob::CameraUpdatePerspectiveProjectionJob(EntityManager* entities, float width, float height) :
    BaseJob("CameraUpdatePerspectiveProjectionJob") {
    m_eventBus = &entities->event_bus();
    m_width = width;
    m_height = height;
    m_listener.attach(m_eventBus);
    m_listener.receive<eagle::OnWindowResized>(this);
    m_cameraGroup.attach(entities);
}

JobResult CameraUpdatePerspectiveProjectionJob::execute() {
    if (!m_windowResized){
        return JobResult::SUCCESS;
    }
    m_windowResized = false;

    for (auto[projection, perspective] : m_cameraGroup){
        perspective->aspectRatio = m_width / m_height;
        projection->matrix = glm::perspective(perspective->fov, perspective->aspectRatio, perspective->near, perspective->far);
        projection->matrix[1][1] *= -1;
        m_eventBus->emit(OnCameraUpdate{projection.owner().id()});
    }
    return JobResult::SUCCESS;
}

bool CameraUpdatePerspectiveProjectionJob::receive(const eagle::OnWindowResized& ev) {
    m_windowResized = true;
    m_width = ev.width;
    m_height = ev.height;
    return false;
}

RenderCameraJob::RenderCameraJob(EntityManager* manager) : BaseJob("RenderCameraJob") {
    m_cameraGroup.attach(manager);
}

JobResult RenderCameraJob::execute() {

    for (auto[camera] : m_cameraGroup){

        auto commandBuffer = camera->commandBuffer;
        commandBuffer->begin();
        for (auto& pass : camera->passes){
            commandBuffer->begin_render_pass(pass.renderPass, pass.framebuffer);
            commandBuffer->execute_commands(pass.commandBuffers);
            commandBuffer->end_render_pass();
        }
        commandBuffer->end();
        camera->context->submit_command_buffer(commandBuffer);
    }
    return JobResult::SUCCESS;
}

void CameraSystem::configure(Engine* engine) {
    updateOrthographicProjectionJob = engine->jobs().enqueue<CameraUpdateOrthographicProjectionJob>(&engine->entities(), 1280, 720);
    updatePerspectiveProjectionJob = engine->jobs().enqueue<CameraUpdatePerspectiveProjectionJob>(&engine->entities(), 1280, 720);
    renderJob = engine->jobs().enqueue<RenderCameraJob>(&engine->entities());
}
