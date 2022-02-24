//
// Created by Ricardo on 4/29/2021.
//

#include <template/ecs/systems/camera_control_system.h>

#include <beagle/ecs/events/camera_events.h>

CameraControlJob::CameraControlJob(beagle::EntityManager* manager, eagle::Timer* timer) {
    m_cameraPositionGroup.attach(manager);
    m_timer = timer;
    m_eventBus = &manager->event_bus();
}

beagle::JobResult CameraControlJob::operator()() {
    auto dt = m_timer->delta_time();

    for (auto[position, rotation, controller, camera] : m_cameraPositionGroup) {
        auto& input = controller->input;
        auto pos = position->vec;
        auto rot = rotation->quat;

        if (input.mouse_button_down(EG_MOUSE_BUTTON_LEFT)){
            if (input.mouse_button_pressed(EG_MOUSE_BUTTON_LEFT)){
                auto[x, y] = input.mouse_position();
                controller->lastMousePosition = glm::vec2(x, y);
            }
            controller->dtAccumulator += dt;
            if (controller->dtAccumulator > 0.008f){
                controller->dtAccumulator -= 0.008f;

                auto[x, y] = input.mouse_position();

                auto dx = x - controller->lastMousePosition.x;
                auto dy = y - controller->lastMousePosition.y;
                controller->lastMousePosition = glm::vec2(x, y);
                if (dx != 0 || dy != 0){

                    float yaw = (float)-dx * dt * controller->mouseSpeed;
                    float pitch = (float)-dy * dt * controller->mouseSpeed;

                    rot = glm::normalize(rot * glm::quat(glm::radians(glm::vec3(pitch, yaw, 0))));
                }
            }
        }

        auto front = rot * glm::vec3(0, 0, controller->speed * dt);
        if (input.key_down(EG_KEY_W)) {
            pos -= front;
        }
        if (input.key_down(EG_KEY_S)) {
            pos += front;
        }

        auto right = rot * glm::vec3(controller->speed * dt, 0, 0);
        if (input.key_down(EG_KEY_D)) {
            pos += right;
        }
        if (input.key_down(EG_KEY_A)) {
            pos -= right;
        }

        auto rollSpeed = glm::radians(glm::vec3(0, 0, controller->rollSpeed * dt));
        if (input.key_down(EG_KEY_E)) {
            rot *= glm::quat(rollSpeed);
            rot = glm::normalize(rot);
        }
        if (input.key_down(EG_KEY_Q)) {
            rot *= glm::quat(-rollSpeed);
            rot = glm::normalize(rot);
        }


        auto up = rot * glm::vec3(0, controller->speed * dt, 0);
        if (input.key_down(EG_KEY_R)) {
            pos += up;
        }
        if (input.key_down(EG_KEY_F)) {
            pos -= up;
        }

        if (position->vec != pos || rotation->quat != rot){
            position->vec = pos;
            rotation->quat = rot;
            m_eventBus->emit(beagle::OnCameraUpdate{camera.owner().id()});
        }

        input.refresh();
    }
    return beagle::JobResult::SUCCESS;
}
