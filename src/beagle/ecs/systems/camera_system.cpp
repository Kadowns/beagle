//
// Created by Ricardo on 4/25/2021.
//

#include <beagle/ecs/components/transform.h>
#include <beagle/ecs/components/mesh_renderer.h>
#include "camera_system.h"

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

void CameraUpdateOrthographicProjectionJob::execute() {
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

void CameraUpdatePerspectiveProjectionJob::execute() {
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

bool CameraUpdatePerspectiveProjectionJob::receive(const eagle::OnWindowResized& ev) {
    m_windowResized = true;
    m_width = ev.width;
    m_height = ev.height;
    return false;
}
