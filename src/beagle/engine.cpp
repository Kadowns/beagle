//
// Created by Ricardo on 3/27/2021.
//

#include "engine.h"

#include <eagle/application.h>
#include <eagle/window.h>

namespace beagle {

struct Rigidbody {

};

void beagle::Engine::init() {

    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_CREATE("beagle");
    EG_LOG_LEVEL(spdlog::level::trace);

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.receive<eagle::OnWindowClose>(this);

    auto context = eagle::Application::instance().window().rendering_context();

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;

    m_commandBuffer = context->create_command_buffer(commandBufferCreateInfo);

    m_vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});
    m_indexBuffer = context->create_index_buffer({eagle::UpdateType::DYNAMIC, eagle::IndexBufferType::UINT_16});

    eagle::ShaderCreateInfo shaderCreateInfo = {
            context->main_render_pass(),
            {
                {eagle::ShaderStage::FRAGMENT, "assets/shaders/color.frag.spv"},
                {eagle::ShaderStage::VERTEX, "assets/shaders/color.vert.spv"},
            }
    };
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32A32_SFLOAT);

    m_shader = context->create_shader(shaderCreateInfo);

    auto e = m_entityManager.create();
    e.assign<Transform>(glm::vec3(0, 0, 0));
    e.assign<Scaler>();

    e = m_entityManager.create();
    auto tr = e.assign<Transform>(glm::vec3(-0.5, 0.5, 0));
    auto osc = e.assign<Oscilator>();
    e.assign<Scaler>();
    osc->anchor = tr->position;

    e = m_entityManager.create();
    tr = e.assign<Transform>(glm::vec3(0.5, -0.5, 0));
    osc = e.assign<Oscilator>();
    osc->frequency = -1;
    osc->anchor = tr->position;


    m_quadsGroup.attach(&m_entityManager);
    m_oscilatorGroup.attach(&m_entityManager);
    m_scalerGroup.attach(&m_entityManager);

    EG_INFO("beagle", "scaler group size {0}", m_scalerGroup.size());
    m_timer.start();

    auto oscilatorJob = m_jobSystem.add_job([this]{
        float t = m_timer.time();

        for (auto[tr, osc] : m_oscilatorGroup){
            tr->position.x = osc->anchor.x + sinf(t * osc->frequency) * osc->amplitude;
        }
    });

    auto scalerJob = m_jobSystem.add_job([this]{
        float t = m_timer.time();

        for (auto[tr, osc] : m_scalerGroup){
            auto amplitude = osc->amplitude;
            auto frequency = osc->frequency;
            auto scale = tr->scale;
            scale = glm::vec3(1) * (sinf(frequency * t) * amplitude);
            tr->scale = scale;
        }
    });

    auto quadJob = m_jobSystem.add_job([this]{

        struct Vertex {
            glm::vec3 position;
            glm::vec4 color;
        };

        struct Quad {
            Vertex vertices[4];
        };

        auto vb = m_vertexBuffer.lock();
        auto ib = m_indexBuffer.lock();
        vb->reserve(m_quadsGroup.size() * sizeof(Quad));
        vb->clear();
        ib->reserve(m_quadsGroup.size() * sizeof(uint16_t) * 6);
        ib->clear();
        int index = 0;
        for (auto[tr] : m_quadsGroup){

            auto position = tr->position;
            auto scale = tr->scale;

            Quad quad = {{
                                 {(glm::vec3(-0.1, -0.1, 0.0f) * scale) + position, glm::vec4(0.5f, 1.0f, 0.5f, 1.0f)},
                                 {(glm::vec3(0.1, -0.1, 0.0f) * scale) + position, glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},
                                 {(glm::vec3(-0.1, 0.1, 0.0f) * scale) + position, glm::vec4(0.5f, 1.0f, 1.0f, 1.0f)},
                                 {(glm::vec3(0.1, 0.1, 0.0f) * scale) + position, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)},
                         }
            };

            uint16_t indices[6] = {
                    0, 1, 2, 1, 3, 2
            };
            indices[0] += index * 4;
            indices[1] += index * 4;
            indices[2] += index * 4;
            indices[3] += index * 4;
            indices[4] += index * 4;
            indices[5] += index * 4;

            vb->insert(&quad, sizeof(Quad));
            ib->insert(indices, sizeof(uint16_t) * 6);
            index++;
        }
        vb->upload();
        ib->upload();
    });
    m_jobSystem.add_dependency(quadJob, oscilatorJob);
    m_jobSystem.add_dependency(quadJob, scalerJob);

    auto renderJob = m_jobSystem.add_job([this]{
        auto context = eagle::Application::instance().window().rendering_context();
        if (!context->prepare_frame()){
            return;
        }
        auto commandBuffer = m_commandBuffer.lock();
        commandBuffer->begin();
        commandBuffer->begin_render_pass(context->main_render_pass(), context->main_frambuffer());
        commandBuffer->bind_shader(m_shader.lock());
        commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
        commandBuffer->bind_index_buffer(m_indexBuffer.lock());
        commandBuffer->draw_indexed(m_quadsGroup.size() * 6, 0, 0);
        commandBuffer->end_render_pass();
        commandBuffer->end();

        context->present_frame(commandBuffer);
    });
    m_jobSystem.add_dependency(renderJob, quadJob);
}

void beagle::Engine::step() {
    m_timer.update();
    m_jobSystem.execute();
}

void beagle::Engine::destroy() {
    m_listener.detach();
}

bool Engine::receive(const eagle::OnWindowClose& ev) {
    eagle::Application::instance().quit();
    return false;
}

}