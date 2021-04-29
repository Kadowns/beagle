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
    m_listener.receive<eagle::OnWindowResized>(this);
    m_game->init(this);
    m_timer.start();
}

void Engine::step() {
    m_timer.update();
    m_game->step(this);
    m_jobSystem.execute();
    wait_for_target_fps(m_timer.delta_time());
}

void Engine::destroy() {
    m_game->destroy(this);
    m_listener.detach();
}

bool Engine::receive(const eagle::OnWindowClose& ev) {
    eagle::Application::instance().quit();
    return false;
}

bool Engine::receive(const eagle::OnWindowResized& ev) {
    m_entityManager.event_bus().emit(ev);
    return false;
}

void Engine::wait_for_target_fps(float dt) {
    const float targetDt = 0.008f;
    if (dt < targetDt){
        int64_t sleepForMs = (targetDt - dt) * 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepForMs));
    }
}

}