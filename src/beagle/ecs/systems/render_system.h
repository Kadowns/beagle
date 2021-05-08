//
// Created by Ricardo on 4/30/2021.
//

#ifndef BEAGLE_RENDER_SYSTEM_H
#define BEAGLE_RENDER_SYSTEM_H

#include <beagle/ecs/job_system.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>

#include <eagle/renderer/rendering_context.h>

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


class RenderCameraJob : public BaseJob {
public:
    explicit RenderCameraJob(EntityManager* manager);
    void execute() override;
private:
    EntityGroup<Camera> m_cameraGroup;
};

}

#endif //TEMPLATE_APP_RENDER_SYSTEM_H
