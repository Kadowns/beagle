//
// Created by Ricardo on 4/24/2021.
//

#ifndef BEAGLE_CAMERA_H
#define BEAGLE_CAMERA_H

#include <beagle/math.h>

#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/render_pass.h>
#include <eagle/renderer/framebuffer.h>
#include <eagle/renderer/rendering_context.h>

namespace beagle {

struct Camera {
    explicit Camera(eagle::RenderingContext* context) :
    context(context), renderPass(context->main_render_pass()), framebuffer(context->main_frambuffer()) {
        eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
        commandBufferCreateInfo.level = eagle::CommandBufferLevel::PRIMARY;
        commandBuffer = context->create_command_buffer(commandBufferCreateInfo);
    }
    eagle::RenderingContext* context;
    std::weak_ptr<eagle::RenderPass> renderPass;
    std::weak_ptr<eagle::Framebuffer> framebuffer;
    std::weak_ptr<eagle::CommandBuffer> commandBuffer;
    std::vector<std::weak_ptr<eagle::CommandBuffer>> secondaryCommandBuffers;
};

struct CameraProjection {
    glm::mat4 matrix;
};

struct CameraPerspectiveProjection {
    CameraPerspectiveProjection(float fov, float aspectRatio, float near, float far) :
    fov(fov), aspectRatio(aspectRatio), near(near), far(far) {}
    float fov;
    float aspectRatio;
    float near;
    float far;
};

struct CameraOrthographicProjection {
    CameraOrthographicProjection(float left, float right, float bottom, float top, float near, float far) :
    left(left), right(right), bottom(bottom), top(top), near(near), far(far) {}
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
};

}

#endif //BEAGLE_CAMERA_H
