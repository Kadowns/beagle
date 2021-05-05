//
// Created by Ricardo on 4/25/2021.
//

#include <beagle/ecs/components/transform.h>
#include "camera_system.h"

using namespace beagle;


CameraUploadSystem::CameraUploadSystem(EntityManager* entities) : BaseJob("CameraUpdateSystem"){
    m_listener.attach(&entities->event_bus());
    m_listener.receive<OnCameraUpdate>(this);
    m_manager = entities;
}

void CameraUploadSystem::execute() {
    if (m_dirtyCameras.empty()){
        return;
    }

    CameraUniform ubo{};

    for (auto entityId : m_dirtyCameras) {
        auto[camera, projection, transform] = m_manager->entity_from_id(entityId).components<Camera, CameraProjection, Transform>();
        ubo.vp = projection->matrix * transform->inverseMatrix;
        auto cameraUbo = camera->ubo.lock();
        cameraUbo->copy_from(&ubo, sizeof(ubo), 0);
        cameraUbo->upload();
    }
    m_dirtyCameras.clear();
}

bool CameraUploadSystem::receive(const OnCameraUpdate& ev) {
    m_dirtyCameras.insert(ev.entityId);
    return false;
}

CameraOrthographicSystem::CameraOrthographicSystem(EntityManager* entities, float width, float height) : BaseJob("CameraOrthographicSystem") {
    m_eventBus = &entities->event_bus();
    m_width = width;
    m_height = height;
    m_listener.attach(m_eventBus);
    m_listener.receive<eagle::OnWindowResized>(this);
    m_cameraGroup.attach(entities);
}

void CameraOrthographicSystem::execute() {
    if (!m_windowResized){
        return;
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
}

bool CameraOrthographicSystem::receive(const eagle::OnWindowResized& ev) {
    m_windowResized = true;
    m_width = ev.width;
    m_height = ev.height;
    return false;
}

CameraPerspectiveSystem::CameraPerspectiveSystem(EntityManager* entities, float width, float height) : BaseJob("CameraPerspectiveSystem") {
    m_eventBus = &entities->event_bus();
    m_width = width;
    m_height = height;
    m_listener.attach(m_eventBus);
    m_listener.receive<eagle::OnWindowResized>(this);
    m_cameraGroup.attach(entities);
}

void CameraPerspectiveSystem::execute() {
    if (!m_windowResized){
        return;
    }
    m_windowResized = false;

    for (auto[projection, perspective] : m_cameraGroup){
        perspective->aspectRatio = m_width / m_height;
        projection->matrix = glm::perspective(perspective->fov, perspective->aspectRatio, perspective->near, perspective->far);
        projection->matrix[1][1] *= -1;
        m_eventBus->emit(OnCameraUpdate{projection.owner().id()});
    }
}

bool CameraPerspectiveSystem::receive(const eagle::OnWindowResized& ev) {
    m_windowResized = true;
    m_width = ev.width;
    m_height = ev.height;
    return false;
}
