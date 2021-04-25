//
// Created by Ricardo on 4/21/2021.
//

#include "template_game.h"
#include <eagle/application.h>
#include <eagle/window.h>

#include <beagle/ecs/systems/transform_system.h>
#include <beagle/ecs/systems/camera_system.h>
#include <beagle/renderer/uniforms/camera_uniform.h>

TemplateGame::TemplateGame() {
    EG_LOG_CREATE("template");
}

void TemplateGame::init(beagle::Engine* engine) {
    EG_TRACE("template", "init called");

    auto& window = eagle::Application::instance().window();
    auto context = window.rendering_context();

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;

    m_commandBuffer = context->create_command_buffer(commandBufferCreateInfo);

    m_vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});
    m_indexBuffer = context->create_index_buffer({eagle::UpdateType::DYNAMIC, eagle::IndexBufferType::UINT_16});

    eagle::ShaderCreateInfo shaderCreateInfo = {
            context->main_render_pass(),
            {
                    {eagle::ShaderStage::FRAGMENT, "shaders/color.frag.spv"},
                    {eagle::ShaderStage::VERTEX, "shaders/color.vert.spv"},
            }
    };
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32A32_SFLOAT);

    m_shader = context->create_shader(shaderCreateInfo);

    auto e = engine->entities().create();
    e.assign<beagle::Position>(1000, 200, 0);
    e.assign<beagle::Rotation>();
    e.assign<beagle::Scale>(2, 2, 2);
    auto rot =e.assign<Rotator>();
    rot->frequency = 90.0f;
    e.assign<beagle::Transform>();
//    e.assign<Scaler>();

    e = engine->entities().create();
    auto pos = e.assign<beagle::Position>(300, 500, 0);
    e.assign<beagle::Scale>(1, 3, 1);
    auto osc = e.assign<Oscilator>();
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = 60.0f;
    e.assign<beagle::Transform>();
//    e.assign<Scaler>();
    osc->anchor = pos->position;
    osc->amplitude = 400;

    e = engine->entities().create();
    pos = e.assign<beagle::Position>(window.width() / 2, window.height() / 2, 0);
    e.assign<beagle::Scale>(0.5, 1, 1);
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = 30.0f;
    e.assign<beagle::Transform>();
    osc = e.assign<Oscilator>();
    osc->frequency = -1;
    osc->anchor = pos->position;
    osc->amplitude = 120;


    e = engine->entities().create();
    e.assign<beagle::Position>(0.0f, 0.0f, 10.0f);
    e.assign<beagle::CameraOrthographicProjection>(0, window.width(), window.height(), 0, 0.1f, 100.0f);
    e.assign<beagle::CameraProjection>();
    e.assign<beagle::CameraView>();
    auto camera = e.assign<beagle::Camera>();
    camera->ubo = context->create_uniform_buffer(sizeof(beagle::CameraUniform), nullptr);
    camera->renderPass = context->main_render_pass();
    camera->framebuffer = context->main_frambuffer();

    m_oscilatorGroup.attach(&engine->entities());
    m_scalerGroup.attach(&engine->entities());
    m_rotatorGroup.attach(&engine->entities());
    m_quadsGroup.attach(&engine->entities());

    m_descriptorSet = context->create_descriptor_set(m_shader.lock()->get_descriptor_set_layout(0).lock(), {camera->ubo.lock()});

    auto rotatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();
        for (auto[rotation, rotator] : m_rotatorGroup){
            rotation->rotation = glm::quat(glm::vec3(0, 0, glm::radians(t * rotator->frequency)));
        }
    });

    auto oscilatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_oscilatorGroup){
            tr->position.x = osc->anchor.x + sinf(t * osc->frequency) * osc->amplitude;
        }
    });

    auto scalerJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_scalerGroup){
            auto amplitude = osc->amplitude;
            auto frequency = osc->frequency;
            auto scale = tr->scale;
            scale = glm::vec3(1) * (sinf(frequency * t) * amplitude);
            tr->scale = scale;
        }
    });

    auto transformJob = engine->jobs().enqueue<beagle::TransformSystem>(&engine->entities());
    transformJob.run_after(scalerJob);
    transformJob.run_after(oscilatorJob);
    transformJob.run_after(rotatorJob);

    auto quadJob = engine->jobs().enqueue<beagle::Job>([this]{

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
        for (auto[t] : m_quadsGroup){

            auto matrix = t->matrix;

            Quad quad = {{
                                 {matrix * glm::vec4(-50.0f, -50.0f, 0.0f, 1.0f), glm::vec4(0.5f, 1.0f, 0.5f, 1.0f)},
                                 {matrix * glm::vec4(50.0f, -50.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},
                                 {matrix * glm::vec4(-50.0f, 50.0f, 0.0f, 1.0f), glm::vec4(0.5f, 1.0f, 1.0f, 1.0f)},
                                 {matrix * glm::vec4(50.0f, 50.0f, 0.0f, 1.0f), glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)},
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
    quadJob.run_after(transformJob);

    auto cameraViewJob = engine->jobs().enqueue<beagle::CameraViewSystem>(&engine->entities());
    auto cameraOrthoJob = engine->jobs().enqueue<beagle::CameraOrthographicSystem>(&engine->entities(), window.width(), window.height());
    auto cameraUploadJob = engine->jobs().enqueue<beagle::CameraUploadSystem>(&engine->entities());
    cameraUploadJob.run_after(cameraViewJob);
    cameraUploadJob.run_after(cameraOrthoJob);

    auto renderJob = engine->jobs().enqueue<beagle::Job>([this]{
        auto context = eagle::Application::instance().window().rendering_context();
        if (!context->prepare_frame()){
            return;
        }
        auto commandBuffer = m_commandBuffer.lock();
        commandBuffer->begin();
        commandBuffer->begin_render_pass(context->main_render_pass(), context->main_frambuffer());
        commandBuffer->bind_shader(m_shader.lock());
        commandBuffer->bind_descriptor_sets(m_descriptorSet.lock(), 0);
        commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
        commandBuffer->bind_index_buffer(m_indexBuffer.lock());
        commandBuffer->draw_indexed(m_quadsGroup.size() * 6, 0, 0);
        commandBuffer->end_render_pass();
        commandBuffer->end();

        context->present_frame(commandBuffer);
    });
    renderJob.run_after(quadJob);
    renderJob.run_after(cameraUploadJob);
}

void TemplateGame::step(beagle::Engine* engine) {

}

void TemplateGame::destroy(beagle::Engine* engine) {
    EG_TRACE("template", "destroy called");
}

