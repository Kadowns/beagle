//
// Created by Ricardo on 4/30/2021.
//

#ifndef BEAGLE_RENDER_SYSTEM_H
#define BEAGLE_RENDER_SYSTEM_H

#include <beagle/ecs/job_system.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>
#include <beagle/ecs/components/renderer.h>

#include <eagle/renderer/rendering_context.h>
#include <beagle/ecs/components/transform.h>

namespace beagle {

class RenderBeginJob : public BaseJob {
public:
    explicit RenderBeginJob(eagle::RenderingContext* context);
    void execute() override;
private:
    eagle::RenderingContext* m_context;
};

class RenderEndJob : public BaseJob {
public:
    explicit RenderEndJob(eagle::RenderingContext* context);
    void execute() override;
private:
    eagle::RenderingContext* m_context;
};

class BuildMeshGroupsJob : public BaseJob {
public:
    BuildMeshGroupsJob(EntityManager* manager);
    void execute() override;
private:
    EntityGroup<Transform, MeshRenderer> m_meshRendererGroup;
    EntityGroup<MeshFilter> m_meshFilterGroup;
};

class RenderMeshFilterJob : public BaseJob {
public:
    RenderMeshFilterJob(EntityManager* manager);
    void execute() override;

private:
    EntityGroup<Camera, MeshFilter> m_meshFilterGroup;

};

class RenderCameraJob : public BaseJob {
public:
    explicit RenderCameraJob(EntityManager* manager);
    void execute() override;
private:
    EntityGroup<Camera> m_cameraGroup;
};

}

#endif //TEMPLATE_APP_RENDER_SYSTEM_H
