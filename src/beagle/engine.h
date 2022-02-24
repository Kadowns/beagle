//
// Created by Ricardo on 3/27/2021.
//

#ifndef BEAGLE_ENGINE_H
#define BEAGLE_ENGINE_H

#include <beagle/beagle_global_definitions.h>
#include <beagle/game.h>
#include <beagle/ecs/thread_pool.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/job_executor.h>
#include <beagle/ecs/components/transform.h>
#include <beagle/assets/asset_manager.h>

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
    bool receive(const eagle::OnWindowResized& ev);

    inline EntityManager& entities() { return m_entityManager; }
    inline AssetManager& assets() { return *m_assetManager; }
    inline eagle::Timer& timer() { return m_timer; }
    inline ThreadPool& thread_pool() { return m_threadPool; }
    inline JobExecutor& executor() { return m_executor; }

private:

    void wait_for_target_fps(float dt);

private:
    ThreadPool m_threadPool;
    JobExecutor m_executor;
    eagle::EventListener m_listener;
    eagle::Timer m_timer;


    EntityManager m_entityManager;
    std::unique_ptr<AssetManager> m_assetManager;

    std::unique_ptr<Game> m_game;

};

}

#endif //BEAGLE_APPLICATION_H
