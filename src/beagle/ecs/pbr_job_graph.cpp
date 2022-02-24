//
// Created by Ricardo on 1/23/2022.
//

#include "pbr_job_graph.h"

#include <beagle/ecs/systems/transform_system.h>
#include <beagle/ecs/systems/skybox_system.h>
#include <beagle/ecs/systems/render_system.h>
#include <beagle/ecs/systems/mesh_system.h>
#include <beagle/ecs/systems/camera_system.h>
#include <eagle/application.h>
#include <eagle/window.h>

using namespace beagle;

PBRJobGraph::PBRJobGraph(Engine* engine) {
    updateTransformJob = graph.emplace<TransformUpdateMatricesJob>(&engine->entities()).name("Transform Update");

    auto& window = eagle::Application::instance().window();
    auto context =  window.rendering_context();
    beginRenderJob = graph.emplace<RenderBeginJob>(context).name("Render Begin");
    endRenderJob = graph.emplace<RenderEndJob>(context).name("Render End");

    skyboxUpdateVertexUboJob = graph.emplace<SkyboxFilterUpdateVertexUboJob>(&engine->entities()).name("Skybox Update Vertex UBO");
    skyboxRenderJob = graph.emplace<SkyboxFilterRenderJob>((&engine->entities())).name("Skybox Render");

    meshUpdateVertexUboJob = graph.emplace<MeshFilterUpdateVertexUboJob>((&engine->entities())).name("Mesh Update Vertex UBO");
    meshUpdateInstanceBufferJob = graph.emplace<MeshFilterUpdateInstanceBufferJob>((&engine->entities())).name("Mesh Update Instance UBO");
    meshUpdateFragmentUboJob = graph.emplace<MeshFilterUpdateFragmentUboJob>((&engine->entities())).name("Mesh Update Fragment UBO");
    meshRenderJob = graph.emplace<MeshFilterRenderJob>((&engine->entities())).name("Mesh Render");

    cameraUpdatePerspectiveProjectionJob = graph.emplace<CameraUpdatePerspectiveProjectionJob>(&engine->entities(), window.width(), window.height()).name("Camera Update Perspective Projection");
    cameraRenderJob = graph.emplace<RenderCameraJob>((&engine->entities())).name("Camera Render");

    meshUpdateInstanceBufferJob.succeed(updateTransformJob);
    meshUpdateVertexUboJob.succeed(updateTransformJob);
    meshUpdateVertexUboJob.succeed(cameraUpdatePerspectiveProjectionJob);

    skyboxUpdateVertexUboJob.succeed(cameraUpdatePerspectiveProjectionJob);
    skyboxUpdateVertexUboJob.succeed(updateTransformJob);

    beginRenderJob.succeed(meshUpdateVertexUboJob);
    beginRenderJob.succeed(meshUpdateFragmentUboJob);
    beginRenderJob.succeed(meshUpdateInstanceBufferJob);
    beginRenderJob.succeed(skyboxUpdateVertexUboJob);

    meshRenderJob.succeed(beginRenderJob);
    skyboxRenderJob.succeed(beginRenderJob);

    cameraRenderJob.succeed(meshRenderJob);
    cameraRenderJob.succeed(skyboxRenderJob);

    endRenderJob.succeed(cameraRenderJob);
}

