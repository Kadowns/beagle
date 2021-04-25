//
// Created by Ricardo on 4/25/2021.
//

#include <beagle/renderer/uniforms/camera_uniform.h>
#include <beagle/ecs/components/transform.h>
#include "camera_system.h"

using namespace beagle;


CameraUploadSystem::CameraUploadSystem(EntityManager* entities) {
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
        auto[camera, projection, view] = m_manager->entity_from_id(entityId).components<Camera, CameraProjection, CameraView>();
        ubo.vp = projection->matrix * view->matrix;
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

CameraOrthographicSystem::CameraOrthographicSystem(EntityManager* entities, float width, float height) {
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

CameraViewSystem::CameraViewSystem(EntityManager* manager) {
    m_manager = manager;
    m_eventBus = &manager->event_bus();
    m_listener.attach(m_eventBus);
    m_listener.receive<OnCameraTransformed>(this);

    EntityGroup<CameraView> cameraGroup;
    cameraGroup.attach(manager);
    for (auto[view] : cameraGroup){
        m_dirtyCameras.insert(view.owner().id());
    }
}

void CameraViewSystem::execute() {
    if (m_dirtyCameras.empty()){
        return;
    }

    for (auto entityId : m_dirtyCameras){
        auto entity = m_manager->entity_from_id(entityId);

        glm::vec3 position;
        glm::quat rotation(glm::vec3(0));
        if (entity.has_component<Position>()){
            position = entity.component<Position>()->position;
        }
        if (entity.has_component<Rotation>()){
            rotation = entity.component<Rotation>()->rotation;
        }
        entity.component<CameraView>()->matrix = glm::lookAt(position, rotation * glm::vec3(0, 0, -1), rotation * glm::vec3(0, 1, 0));
        m_eventBus->emit(OnCameraUpdate{entityId});
    }
    m_dirtyCameras.clear();
}

bool CameraViewSystem::receive(const OnCameraTransformed& ev) {
    m_dirtyCameras.insert(ev.entityId);
    return false;
}

bool CameraViewSystem::receive(const OnComponentAdded<CameraView>& ev) {
    m_dirtyCameras.insert(ev.component.owner().id());
    return false;
}
