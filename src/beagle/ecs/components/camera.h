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

#include <utility>

namespace beagle {

struct Camera {

    struct Pass {
        std::shared_ptr<eagle::RenderPass> renderPass;
        std::shared_ptr<eagle::Framebuffer> framebuffer;
        std::vector<std::shared_ptr<eagle::CommandBuffer>> commandBuffers;
    };

    explicit Camera(eagle::RenderingContext* context) :
    context(context) {
        eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
        commandBufferCreateInfo.level = eagle::CommandBufferLevel::PRIMARY;
        commandBuffer = context->create_command_buffer(commandBufferCreateInfo);
    }

    eagle::RenderingContext* context;
    std::shared_ptr<eagle::CommandBuffer> commandBuffer;
    std::vector<Pass> passes;
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
