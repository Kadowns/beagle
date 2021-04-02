//
// Created by Ricardo on 3/27/2021.
//

#include "engine.h"

#include <eagle/application.h>
#include <eagle/window.h>

namespace beagle {

Engine::Engine() : m_entityManager(m_entityEventBus), m_physicsGroup(&m_entityManager){

}

void beagle::Engine::init() {

    auto context = eagle::Application::instance().window().rendering_context();

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.updateType = eagle::UpdateType::DYNAMIC;
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;

    m_commandBuffer = context->create_command_buffer(commandBufferCreateInfo);

    auto e = m_entityManager.create();
    auto tr = e.assign<Transform>();
    tr->position[0] = 0;
    tr->position[1] = 0;
    tr->position[2] = 0;

    tr->rotation[0] = 0;
    tr->rotation[1] = 0;
    tr->rotation[2] = 0;
    tr->rotation[3] = 1;

    tr->scale[0] = 1;
    tr->scale[1] = 1;
    tr->scale[2] = 1;

    auto rb = e.assign<Rigidbody>();
    rb->velocity[0] = 10;
    rb->velocity[1] = 5;
    rb->velocity[2] = 0;

}

void beagle::Engine::step() {

    for (auto entity : m_physicsGroup){
        auto tr = entity.component<Transform>();
        auto rb = entity.component<Rigidbody>();

        tr->position[0] += rb->velocity[0];
        tr->position[1] += rb->velocity[1];
        tr->position[2] += rb->velocity[2];
    }

    auto context = eagle::Application::instance().window().rendering_context();
    if (!context->prepare_frame()){
        return;
    }
    auto commandBuffer = m_commandBuffer.lock();
    commandBuffer->begin();
    commandBuffer->begin_render_pass(context->main_render_pass(), context->main_frambuffer());
    commandBuffer->end_render_pass();
    commandBuffer->end();

    context->present_frame(commandBuffer);
}

void beagle::Engine::destroy() {

}

}