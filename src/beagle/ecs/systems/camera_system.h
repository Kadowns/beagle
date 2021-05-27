//
// Created by Ricardo on 4/25/2021.
//

#ifndef BEAGLE_CAMERA_SYSTEM_H
#define BEAGLE_CAMERA_SYSTEM_H

#include <beagle/ecs/job_manager.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/events/camera_events.h>

#include <eagle/events/window_events.h>
#include <beagle/ecs/system_manager.h>

namespace beagle {


class CameraUpdateOrthographicProjectionJob : public BaseJob {
public:
    CameraUpdateOrthographicProjectionJob(EntityManager* entities, float width, float height);
    JobResult execute() override;

    bool receive(const eagle::OnWindowResized& ev);

private:
    eagle::EventListener m_listener;
    eagle::EventBus* m_eventBus;
    EntityGroup<CameraProjection, CameraOrthographicProjection> m_cameraGroup;
    float m_width, m_height;
    bool m_windowResized = true;
};

class CameraUpdatePerspectiveProjectionJob : public BaseJob {
public:
    CameraUpdatePerspectiveProjectionJob(EntityManager* entities, float width, float height);
    JobResult execute() override;

    bool receive(const eagle::OnWindowResized& ev);

private:
    eagle::EventListener m_listener;
    eagle::EventBus* m_eventBus;
    EntityGroup<CameraProjection, CameraPerspectiveProjection> m_cameraGroup;
    float m_width, m_height;
    bool m_windowResized = true;
};


class RenderCameraJob : public BaseJob {
public:
    explicit RenderCameraJob(EntityManager* manager);
    JobResult execute() override;
private:
    EntityGroup<Camera> m_cameraGroup;
};

struct CameraSystem : BaseSystem {
    void configure(Engine* engine) override;
    JobManager::JobHandle updateOrthographicProjectionJob;
    JobManager::JobHandle updatePerspectiveProjectionJob;
    JobManager::JobHandle renderJob;
};


}

#endif //BEAGLE_CAMERA_SYSTEM_H
