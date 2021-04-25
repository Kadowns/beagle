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

private:

    void wait_for_target_fps(float dt);

private:
    eagle::EventListener m_listener;
    eagle::Timer m_timer;


    EntityManager m_entityManager;

    JobSystem m_jobSystem;
    std::unique_ptr<Game> m_game;

};

}

#endif //BEAGLE_APPLICATION_H
