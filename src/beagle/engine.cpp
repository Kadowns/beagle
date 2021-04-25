//
// Created by Ricardo on 3/27/2021.
//

#include "engine.h"

#include <eagle/application.h>
#include <eagle/window.h>

namespace beagle {

Engine::Engine(Game* game) : m_game(game){

}

void Engine::init() {

    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_CREATE("beagle");
    EG_LOG_LEVEL(spdlog::level::trace);

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.receive<eagle::OnWindowClose>(this);

    m_timer.start();

    m_game->init(this);
}

void Engine::step() {
    const float targetDt = 0.01f;
    m_timer.update();
    float dt = m_timer.delta_time();
    m_game->step(this);
    m_jobSystem.execute();

    if (dt < targetDt){
        int64_t sleepForMs = (targetDt - dt) * 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepForMs));
    }
}

void Engine::destroy() {
    m_game->destroy(this);
    m_listener.detach();
}

bool Engine::receive(const eagle::OnWindowClose& ev) {
    eagle::Application::instance().quit();
    return false;
}

}