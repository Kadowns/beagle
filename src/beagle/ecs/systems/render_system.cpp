//
// Created by Ricardo on 4/30/2021.
//

#include "render_system.h"
#include <beagle/engine.h>

#include <eagle/application.h>
#include <eagle/window.h>

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

void RenderSystem::configure(Engine* engine) {
    auto context = eagle::Application::instance().window().rendering_context();
    beginJob = engine->jobs().enqueue<RenderBeginJob>(context);
    endJob = engine->jobs().enqueue<RenderEndJob>(context);
}
