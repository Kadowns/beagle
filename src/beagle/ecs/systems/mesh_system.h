//
// Created by Ricardo on 5/8/2021.
//

#ifndef BEAGLE_MESH_SYSTEM_H
#define BEAGLE_MESH_SYSTEM_H

#include <beagle/ecs/job_manager.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/mesh_renderer.h>
#include <beagle/ecs/components/transform.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/components/light.h>
#include <beagle/ecs/events/camera_events.h>
#include <beagle/ecs/system_manager.h>

namespace beagle {

class MeshFilterUpdateVertexUboJob : public BaseJob {
public:
    explicit MeshFilterUpdateVertexUboJob(EntityManager* entities);
    JobResult execute() override;

    bool receive(const OnCameraUpdate& ev);

private:
    eagle::EventListener m_listener;
    EntityManager* m_manager;
    std::set<Entity::Id> m_dirtyCameras;
};


class MeshFilterUpdateInstanceBufferJob : public BaseJob {
public:
    explicit MeshFilterUpdateInstanceBufferJob(EntityManager* manager);

    JobResult execute() override;

private:
    EntityGroup<Transform, MeshRenderer> m_meshRendererGroup;
    EntityGroup<MeshFilter> m_meshFilterGroup;
};

class MeshFilterUpdateFragmentUboJob : public BaseJob {
public:
    explicit MeshFilterUpdateFragmentUboJob(EntityManager* manager);
    JobResult execute() override;

private:
    EntityManager* m_manager;
    EntityGroup<Position, MeshFilter> m_meshFilterGroup;
    EntityGroup<DirectionalLight, Rotation> m_directionalLightGroup;
    EntityGroup<PointLight, Position> m_pointLightGroup;
};


class MeshFilterRenderJob : public BaseJob {
public:
    explicit MeshFilterRenderJob(EntityManager* manager);

    JobResult execute() override;

private:
    EntityGroup<Camera, MeshFilter> m_meshFilterGroup;

};

struct MeshFilterSystem : BaseSystem {
    void configure(Engine* engine) override;

    JobManager::JobHandle updateVertexUboJob;
    JobManager::JobHandle updateInstanceBufferJob;
    JobManager::JobHandle updateFragmentUboJob;
    JobManager::JobHandle renderJob;
};

}

#endif //BEAGLE_MESH_SYSTEM_H
