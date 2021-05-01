//
// Created by Ricardo on 4/21/2021.
//

#include <template/template_game.h>
#include <template/ecs/systems/camera_control_system.h>
#include <eagle/application.h>
#include <eagle/window.h>

#include <beagle/ecs/systems/transform_system.h>
#include <beagle/ecs/systems/camera_system.h>
#include <beagle/ecs/systems/render_system.h>

TemplateGame::TemplateGame() {
    EG_LOG_CREATE("template");
}

void TemplateGame::init(beagle::Engine* engine) {
    EG_TRACE("template", "init called");

    auto& window = eagle::Application::instance().window();
    auto context = window.rendering_context();

    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
    };

    struct Cube {
        Vertex vertices[24];
    };

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

    uint32_t indices[36] = {
            0, 3, 2, 0, 2, 1, //up
            4, 5, 6, 4, 6, 7, //down
            8, 9, 10, 8, 10, 11, //left
            12, 14, 13, 12, 15, 14, //right
            16, 18, 17, 16, 19, 18, //front
            20, 21, 22, 20, 22, 23, //down
    };

    auto cubeMesh = engine->mesh_pool().insert(&cube, sizeof(cube), indices, 36 * sizeof(uint32_t), 36);

    engine->mesh_pool().upload();


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
    e.assign<beagle::MeshRenderer>(cubeMesh, m_shader.lock());
//    e.assign<Scaler>();

    e = engine->entities().create();
    auto pos = e.assign<beagle::Position>(-1, 1, 2);
    e.assign<beagle::Scale>();
    auto osc = e.assign<Oscilator>();
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = glm::vec3(60, 40, 20);
    e.assign<beagle::Transform>();
    e.assign<beagle::MeshRenderer>(cubeMesh, m_shader.lock());
////    e.assign<Scaler>();
    osc->anchor = pos->vec;
    osc->amplitude = 4;

    e = engine->entities().create();
    pos = e.assign<beagle::Position>(-2, 0, 0);
    e.assign<beagle::Scale>();
    e.assign<beagle::Rotation>();
    rot = e.assign<Rotator>();
    rot->frequency = glm::vec3(90, 45, 22.5f);
    e.assign<beagle::Transform>();
    e.assign<beagle::MeshRenderer>(cubeMesh, m_shader.lock());
    osc = e.assign<Oscilator>();
    osc->frequency = -1;
    osc->anchor = pos->vec;
    osc->amplitude = 2;


    e = engine->entities().create();
    e.assign<beagle::Position>(0.0f, 0.0f, 20.0f);
    e.assign<beagle::Rotation>();
    e.assign<beagle::CameraPerspectiveProjection>(glm::radians(45.0f), window.width() / window.height(), 0.1f, 1000.0f);
    e.assign<beagle::CameraProjection>();
    e.assign<beagle::Transform>();
    e.assign<beagle::Camera>(context);
    e.assign<beagle::MeshFilter>(context, &engine->mesh_pool());
    auto controller = e.assign<CameraController>(&eagle::Application::instance().event_bus());
    controller->speed = 10.0f;
    controller->mouseSpeed = 6.0f;


    m_oscilatorGroup.attach(&engine->entities());
    m_scalerGroup.attach(&engine->entities());
    m_rotatorGroup.attach(&engine->entities());
    m_quadsGroup.attach(&engine->entities());


    auto rotatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();
        for (auto[rotation, rotator] : m_rotatorGroup){
            rotation->quat = glm::quat(glm::radians(t * rotator->frequency));
        }
    });

    auto oscilatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_oscilatorGroup){
            tr->vec.x = osc->anchor.x + sinf(t * osc->frequency) * osc->amplitude;
        }
    });

    auto scalerJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_scalerGroup){
            auto amplitude = osc->amplitude;
            auto frequency = osc->frequency;
            auto scale = tr->vec;
            scale = glm::vec3(1) * (sinf(frequency * t) * amplitude);
            tr->vec = scale;
        }
    });

    auto transformJob = engine->jobs().enqueue<beagle::TransformSystem>(&engine->entities());
    transformJob.run_after(scalerJob);
    transformJob.run_after(oscilatorJob);
    transformJob.run_after(rotatorJob);

    auto buildMeshData = engine->jobs().enqueue<beagle::BuildMeshGroupsJob>(&engine->entities());
    buildMeshData.run_after(transformJob);

    auto cameraControllerJob = engine->jobs().enqueue<CameraControlSystem>(&engine->entities(), &engine->timer());
    cameraControllerJob.run_before(transformJob);

    auto cameraOrthoJob = engine->jobs().enqueue<beagle::CameraOrthographicSystem>(&engine->entities(), window.width(), window.height());
    auto cameraPerspectiveJob = engine->jobs().enqueue<beagle::CameraPerspectiveSystem>(&engine->entities(), window.width(), window.height());
    auto cameraUploadJob = engine->jobs().enqueue<beagle::CameraUploadSystem>(&engine->entities());
    cameraUploadJob.run_after(cameraOrthoJob);
    cameraUploadJob.run_after(cameraPerspectiveJob);
    cameraUploadJob.run_after(transformJob);


    auto renderBeginJob = engine->jobs().enqueue<beagle::RenderBeginJob>(context);
    renderBeginJob.run_after(buildMeshData);
    renderBeginJob.run_after(cameraUploadJob);

    auto renderMeshFilter = engine->jobs().enqueue<beagle::RenderMeshFilterJob>(&engine->entities());
    renderMeshFilter.run_after(renderBeginJob);

    auto renderCameraJob = engine->jobs().enqueue<beagle::RenderCameraJob>(&engine->entities());
    renderCameraJob.run_after(renderMeshFilter);

    auto renderEndJob = engine->jobs().enqueue<beagle::RenderEndJob>(context);
    renderEndJob.run_after(renderCameraJob);
}

void TemplateGame::step(beagle::Engine* engine) {

}

void TemplateGame::destroy(beagle::Engine* engine) {
    EG_TRACE("template", "destroy called");
}

