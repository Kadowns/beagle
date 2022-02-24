//
// Created by Ricardo on 4/30/2021.
//

#ifndef BEAGLE_RENDER_SYSTEM_H
#define BEAGLE_RENDER_SYSTEM_H

#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/camera.h>

#include <eagle/renderer/rendering_context.h>
#include <beagle/ecs/job_graph.h>

namespace beagle {

class RenderBeginJob {
public:
    explicit RenderBeginJob(eagle::RenderingContext* context);
    JobResult operator()();
private:
    eagle::RenderingContext* m_context;
};

class RenderEndJob {
public:
    explicit RenderEndJob(eagle::RenderingContext* context);
    JobResult operator()();
private:
    eagle::RenderingContext* m_context;
};

}

#endif //TEMPLATE_APP_RENDER_SYSTEM_H
