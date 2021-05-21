//
// Created by Ricardo on 4/29/2021.
//

#ifndef TEMPLATE_APP_CAMERA_CONTROLLER_H
#define TEMPLATE_APP_CAMERA_CONTROLLER_H

#include <eagle/input.h>
#include <beagle/math.h>

struct CameraController {

    explicit CameraController(eagle::EventBus* eventBus) {
        input.init(eventBus);
    }

    ~CameraController(){
        input.destroy();
    }

    float speed = 1.0f;
    float rollSpeed = 30.0f;
    float mouseSpeed = 1.0f;
    float yaw = 0, pitch = 0;
    float dtAccumulator = 0;
    glm::vec2 lastMousePosition;

    eagle::Input input;
};

#endif //TEMPLATE_APP_CAMERA_CONTROLLER_H
