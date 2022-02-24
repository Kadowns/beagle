//
// Created by Ricardo on 5/8/2021.
//

#ifndef BEAGLE_MESH_SYSTEM_H
#define BEAGLE_MESH_SYSTEM_H


#include <beagle/ecs/entity.h>
#include <beagle/ecs/job_graph.h>
#include <beagle/ecs/components/mesh_renderer.h>
#include <beagle/ecs/components/transform.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/components/light.h>
#include <beagle/ecs/events/camera_events.h>


namespace beagle {

class MeshFilterUpdateVertexUboJob {
public:
    explicit MeshFilterUpdateVertexUboJob(EntityManager* entities);
    JobResult operator()();

    bool receive(const OnCameraUpdate& ev);

private:
    eagle::EventListener m_listener;
    EntityManager* m_manager;
    std::set<Entity::Id> m_dirtyCameras;
};


class MeshFilterUpdateInstanceBufferJob  {
public:
    explicit MeshFilterUpdateInstanceBufferJob(EntityManager* manager);

    JobResult operator()();

private:
    EntityGroup<Transform, MeshRenderer> m_meshRendererGroup;
    EntityGroup<MeshFilter> m_meshFilterGroup;
};

class MeshFilterUpdateFragmentUboJob {
public:
    explicit MeshFilterUpdateFragmentUboJob(EntityManager* manager);
    JobResult operator()();

private:
    EntityManager* m_manager;
    EntityGroup<Position, MeshFilter> m_meshFilterGroup;
    EntityGroup<DirectionalLight, Rotation> m_directionalLightGroup;
    EntityGroup<PointLight, Position> m_pointLightGroup;
};


class MeshFilterRenderJob {
public:
    explicit MeshFilterRenderJob(EntityManager* manager);

    JobResult operator()();

private:
    EntityGroup<Camera, MeshFilter> m_meshFilterGroup;

};

}

#endif //BEAGLE_MESH_SYSTEM_H
