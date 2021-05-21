//
// Created by Ricardo on 5/16/2021.
//

#ifndef BEAGLE_SKYBOX_SYSTEM_H
#define BEAGLE_SKYBOX_SYSTEM_H

#include <beagle/ecs/system_manager.h>
#include <beagle/ecs/job_manager.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/components/skybox.h>
#include <beagle/ecs/events/camera_events.h>

namespace beagle {

class SkyboxFilterUpdateVertexUboJob : public BaseJob {
public:
    explicit SkyboxFilterUpdateVertexUboJob(EntityManager* entities);
    void execute() override;

    bool receive(const OnCameraUpdate& ev);

private:
    eagle::EventListener m_listener;
    EntityManager* m_manager;
    std::set<Entity::Id> m_dirtyCameras;
};

class SkyboxFilterRenderJob : public BaseJob {
public:
    explicit SkyboxFilterRenderJob(EntityManager* manager);
    void execute() override;
private:
    EntityGroup<Camera, SkyboxFilter> m_filterGroup;
};

struct SkyboxFilterSystem : BaseSystem {
    void configure(Engine* engine) override;
    JobManager::JobHandle updateVertexUboJob;
    JobManager::JobHandle renderJob;
};


}

#endif //BEAGLE_SKYBOX_SYSTEM_H
