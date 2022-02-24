//
// Created by Ricardo on 5/16/2021.
//

#ifndef BEAGLE_SKYBOX_SYSTEM_H
#define BEAGLE_SKYBOX_SYSTEM_H


#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/components/skybox.h>
#include <beagle/ecs/events/camera_events.h>
#include <beagle/ecs/job_graph.h>

namespace beagle {

class SkyboxFilterUpdateVertexUboJob {
public:
    explicit SkyboxFilterUpdateVertexUboJob(EntityManager* entities);
    JobResult operator()();

    bool receive(const OnCameraUpdate& ev);

private:
    eagle::EventListener m_listener;
    EntityManager* m_manager;
    std::set<Entity::Id> m_dirtyCameras;
};

class SkyboxFilterRenderJob {
public:
    explicit SkyboxFilterRenderJob(EntityManager* manager);
    JobResult operator()();
private:
    EntityGroup<Camera, SkyboxFilter> m_filterGroup;
};

}

#endif //BEAGLE_SKYBOX_SYSTEM_H
