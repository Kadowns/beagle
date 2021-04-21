//
// Created by Ricardo on 3/27/2021.
//

#ifndef BEAGLE_ENGINE_H
#define BEAGLE_ENGINE_H

#include <beagle/beagle_global_definitions.h>
#include <beagle/game.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/job_system.h>
#include <beagle/ecs/components/transform.h>

#include <eagle/application_delegate.h>
#include <eagle/timer.h>

#include <eagle/renderer/rendering_context.h>
#include <eagle/renderer/command_buffer.h>
#include <eagle/events/window_events.h>

namespace beagle {

class Engine : public eagle::ApplicationDelegate {
public:

    explicit Engine(Game* game);

    void init() override;

    void step() override;

    void destroy() override;

    bool receive(const eagle::OnWindowClose& ev);

    inline EntityManager& entities() { return m_entityManager; }
    inline JobSystem& jobs() { return m_jobSystem; }
    inline eagle::Timer& timer() { return m_timer; }

    inline size_t quad_job_id() const { return m_quadJobId; }
    inline size_t render_job_id() const { return m_renderJobId; }

private:
    eagle::EventListener m_listener;
    eagle::Timer m_timer;
    std::weak_ptr<eagle::CommandBuffer> m_commandBuffer;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::Shader> m_shader;

    EntityManager m_entityManager;
    EntityGroup<Transform> m_quadsGroup;

    JobSystem m_jobSystem;
    std::unique_ptr<Game> m_game;

    size_t m_quadJobId{};
    size_t m_renderJobId{};

};

}

#endif //BEAGLE_APPLICATION_H
