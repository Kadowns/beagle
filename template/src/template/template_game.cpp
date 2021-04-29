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

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
    };

    struct Cube {
        Vertex vertices[24];
    };

    auto vb = m_vertexBuffer.lock();
    auto ib = m_indexBuffer.lock();

    Cube cube = {{
                         //up
                         {glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)},

                         //down
                         {glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f)},
                         {glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.5f, 1.0f, 1.0f)},

                         //left
                         {glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
                         {glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
                         {glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},
                         {glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)},

                         //right
                         {glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)},

                         //front
                         {glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.5f, 1.0f)},

                         //back
                         {glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.5f, 1.0f)},
                         {glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.5f, 1.0f)},
                         {glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, 0.6f, 0.5f, 1.0f)},
                 }};

    uint16_t indices[36] = {
            0, 3, 2, 0, 2, 1, //up
            4, 5, 6, 4, 6, 7, //down
            8, 9, 10, 8, 10, 11, //left
            12, 14, 13, 12, 15, 14, //right
            16, 18, 17, 16, 19, 18, //front
            20, 21, 22, 20, 22, 23, //down
    };

    m_vertexBuffer = context->create_vertex_buffer({eagle::UpdateType::BAKED, sizeof(Cube), &cube});
    m_indexBuffer = context->create_index_buffer({eagle::UpdateType::BAKED, eagle::IndexBufferType::UINT_16, sizeof(uint16_t) * 36, indices});
    m_instanceVertexBuffer = context->create_vertex_buffer({eagle::UpdateType::DYNAMIC});

    eagle::ShaderCreateInfo shaderCreateInfo = {
            context->main_render_pass(),
            {
                    {eagle::ShaderStage::FRAGMENT, "shaders/color.frag.spv"},
                    {eagle::ShaderStage::VERTEX, "shaders/color.vert.spv"},
            }
    };
    //position
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);

    //color
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32A32_SFLOAT);

    //transform matrix
    //build a mat4 using 4 vec4
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout[1].inputRate = eagle::VertexInputRate::INSTANCE;
    shaderCreateInfo.depthTesting = true;

    m_shader = context->create_shader(shaderCreateInfo);

    auto e = engine->entities().create();
    e.assign<beagle::Position>(1, -1, -1);
    e.assign<beagle::Rotation>();
    e.assign<beagle::Scale>();
    auto rot = e.assign<Rotator>();
    rot->frequency = glm::vec3(30, 15, 7.);
    e.assign<beagle::Transform>();
//    e.assign<Scaler>();

    e = engine->entities().create();
    auto pos = e.assign<beagle::Position>(-1, 1, 2);
    e.assign<beagle::Scale>();
    auto osc = e.assign<Oscilator>();
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = glm::vec3(60, 40, 20);
    e.assign<beagle::Transform>();
////    e.assign<Scaler>();
    osc->anchor = pos->position;
    osc->amplitude = 4;

    e = engine->entities().create();
    pos = e.assign<beagle::Position>(-2, 0, 0);
    e.assign<beagle::Scale>();
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = glm::vec3(90, 45, 22.5f);
    e.assign<beagle::Transform>();
    osc = e.assign<Oscilator>();
    osc->frequency = -1;
    osc->anchor = pos->position;
    osc->amplitude = 2;


    e = engine->entities().create();
    e.assign<beagle::Position>(0.0f, 0.0f, 20.0f);
//    e.assign<beagle::CameraOrthographicProjection>(0, 20, 12, 0, 0.1f, 100.0f);
    e.assign<beagle::CameraPerspectiveProjection>(glm::radians(45.0f), window.width() / window.height(), 0.1f, 1000.0f);
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
            rotation->rotation = glm::quat(glm::radians(t * rotator->frequency));
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

    auto instanceMatrixJob = engine->jobs().enqueue<beagle::Job>([this]{
        auto ivb = m_instanceVertexBuffer.lock();
        ivb->reserve(m_quadsGroup.size() * sizeof(glm::mat4));
        ivb->clear();
        for (auto[t] : m_quadsGroup){
            ivb->insert(&t->matrix, sizeof(glm::mat4));
        }
        ivb->upload();
    });
    instanceMatrixJob.run_after(transformJob);

    auto cameraViewJob = engine->jobs().enqueue<beagle::CameraViewSystem>(&engine->entities());
    auto cameraOrthoJob = engine->jobs().enqueue<beagle::CameraOrthographicSystem>(&engine->entities(), window.width(), window.height());
    auto cameraPerspectiveJob = engine->jobs().enqueue<beagle::CameraPerspectiveSystem>(&engine->entities(), window.width(), window.height());
    auto cameraUploadJob = engine->jobs().enqueue<beagle::CameraUploadSystem>(&engine->entities());
    cameraUploadJob.run_after(cameraViewJob);
    cameraUploadJob.run_after(cameraOrthoJob);
    cameraUploadJob.run_after(cameraPerspectiveJob);

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
        commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock(), 0);
        commandBuffer->bind_vertex_buffer(m_instanceVertexBuffer.lock(), 1);
        commandBuffer->bind_index_buffer(m_indexBuffer.lock());
        commandBuffer->draw_indexed(36, m_quadsGroup.size(), 0, 0, 0);
        commandBuffer->end_render_pass();
        commandBuffer->end();

        context->present_frame(commandBuffer);
    });
    renderJob.run_after(instanceMatrixJob);
    renderJob.run_after(cameraUploadJob);
}

void TemplateGame::step(beagle::Engine* engine) {

}

void TemplateGame::destroy(beagle::Engine* engine) {
    EG_TRACE("template", "destroy called");
}

