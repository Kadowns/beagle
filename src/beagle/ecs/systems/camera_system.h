//
// Created by Ricardo on 4/25/2021.
//

#ifndef BEAGLE_CAMERA_SYSTEM_H
#define BEAGLE_CAMERA_SYSTEM_H

#include <beagle/ecs/job_system.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/events/camera_events.h>

#include <eagle/events/window_events.h>

namespace beagle {

class CameraUploadSystem : public BaseJob {
public:
    CameraUploadSystem(EntityManager* entities);
    void execute() override;

    bool receive(const OnCameraUpdate& ev);

private:
    eagle::EventListener m_listener;
    EntityManager* m_manager;
    std::set<Entity::Id> m_dirtyCameras;

};


class CameraOrthographicSystem : public BaseJob {
public:
    CameraOrthographicSystem(EntityManager* entities, float width, float height);
    void execute() override;

    bool receive(const eagle::OnWindowResized& ev);

private:
    eagle::EventListener m_listener;
    eagle::EventBus* m_eventBus;
    EntityGroup<CameraProjection, CameraOrthographicProjection> m_cameraGroup;
    float m_width, m_height;
    bool m_windowResized = true;
};

class CameraPerspectiveSystem : public BaseJob {
public:
    CameraPerspectiveSystem(EntityManager* entities, float width, float height);
    void execute() override;

    bool receive(const eagle::OnWindowResized& ev);

private:
    eagle::EventListener m_listener;
    eagle::EventBus* m_eventBus;
    EntityGroup<CameraProjection, CameraPerspectiveProjection> m_cameraGroup;
    float m_width, m_height;
    bool m_windowResized = true;
};

class CameraViewSystem : public BaseJob {
public:
    explicit CameraViewSystem(EntityManager* manager);
    void execute() override;

    bool receive(const OnCameraTransformed& ev);
    bool receive(const OnComponentAdded<CameraView>& ev);

private:
    EntityManager* m_manager;
    eagle::EventBus* m_eventBus;
    eagle::EventListener m_listener;
    std::set<Entity::Id> m_dirtyCameras;

};


}

#endif //BEAGLE_CAMERA_SYSTEM_H
