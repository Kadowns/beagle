//
// Created by Ricardo on 3/27/2021.
//

#ifndef BEAGLE_ENGINE_H
#define BEAGLE_ENGINE_H

#include <beagle/ecs/entity.h>
#include <beagle/ecs/job_system.h>
#include <beagle/ecs/components/transform.h>

#include <eagle/application_delegate.h>
#include <eagle/timer.h>

#include <eagle/renderer/rendering_context.h>
#include <eagle/renderer/command_buffer.h>
#include <eagle/events/window_events.h>

namespace beagle {

struct Oscilator {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    glm::vec3 anchor;
};

struct Scaler {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    glm::vec3 anchor;
};

class Engine : public eagle::ApplicationDelegate {
public:

    void init() override;

    void step() override;

    void destroy() override;

    bool receive(const eagle::OnWindowClose& ev);

private:
    eagle::EventListener m_listener;
    eagle::Timer m_timer;
    std::weak_ptr<eagle::CommandBuffer> m_commandBuffer;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::Shader> m_shader;



    EntityManager m_entityManager;
    EntityGroup<Transform> m_quadsGroup;
    EntityGroup<Transform, Oscilator> m_oscilatorGroup;
    EntityGroup<Transform, Scaler> m_scalerGroup;

    JobSystem m_jobSystem;

};

}

#endif //BEAGLE_APPLICATION_H
