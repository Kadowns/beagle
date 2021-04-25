//
// Created by Ricardo on 4/24/2021.
//

#ifndef BEAGLE_CAMERA_H
#define BEAGLE_CAMERA_H

#include <beagle/math.h>

#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/render_pass.h>
#include <eagle/renderer/framebuffer.h>

namespace beagle {

struct Camera {
    std::weak_ptr<eagle::UniformBuffer> ubo;
    std::weak_ptr<eagle::RenderPass> renderPass;
    std::weak_ptr<eagle::Framebuffer> framebuffer;
};

struct CameraProjection {
    glm::mat4 matrix;
};

struct CameraView {
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
