//
// Created by Ricardo on 4/30/2021.
//

#include "render_system.h"

#include <utility>

using namespace beagle;

RenderBeginJob::RenderBeginJob(eagle::RenderingContext* context) : m_context(context), BaseJob("RenderBeginJob") {

}

void RenderBeginJob::execute() {
    if (!m_context->prepare_frame()){
        return;
    }
}

RenderEndJob::RenderEndJob(eagle::RenderingContext* context) : m_context(context), BaseJob("RenderEndJob") {

}

void RenderEndJob::execute() {
    m_context->present_frame();
}


RenderCameraJob::RenderCameraJob(EntityManager* manager) : BaseJob("RenderCameraJob") {
    m_cameraGroup.attach(manager);
}

void RenderCameraJob::execute() {

    for (auto[camera] : m_cameraGroup){

        auto commandBuffer = camera->commandBuffer.lock();
        commandBuffer->begin();
        commandBuffer->begin_render_pass(camera->renderPass.lock(), camera->framebuffer.lock());
        commandBuffer->execute_commands(camera->secondaryCommandBuffers);
        commandBuffer->end_render_pass();
        commandBuffer->end();
        camera->context->submit_command_buffer(commandBuffer);
        camera->secondaryCommandBuffers.clear();
    }
}
