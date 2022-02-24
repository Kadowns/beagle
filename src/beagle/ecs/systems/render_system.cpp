//
// Created by Ricardo on 4/30/2021.
//

#include "render_system.h"
#include <beagle/engine.h>

#include <eagle/application.h>
#include <eagle/window.h>

using namespace beagle;

RenderBeginJob::RenderBeginJob(eagle::RenderingContext* context) : m_context(context) {

}

JobResult RenderBeginJob::operator()() {
    if (!m_context->prepare_frame()){
        return JobResult::INTERRUPT;
    }
    return JobResult::SUCCESS;
}

RenderEndJob::RenderEndJob(eagle::RenderingContext* context) : m_context(context) {

}

JobResult RenderEndJob::operator()() {
    m_context->present_frame();
    return JobResult::SUCCESS;
}
