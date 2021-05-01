//
// Created by Ricardo on 4/29/2021.
//

#include <template/ecs/systems/camera_control_system.h>

#include <beagle/ecs/events/camera_events.h>

CameraControlSystem::CameraControlSystem(beagle::EntityManager* manager, eagle::Timer* timer) {
    m_cameraPositionGroup.attach(manager);
    m_timer = timer;
    m_eventBus = &manager->event_bus();
}

void CameraControlSystem::execute() {

    auto dt = m_timer->delta_time();

    for (auto[position, rotation, controller, camera] : m_cameraPositionGroup) {
        auto& input = controller->input;
        auto pos = position->vec;
        auto rot = rotation->quat;

        if (input.mouse_button_down(EG_MOUSE_BUTTON_LEFT)){
            auto[dx, dy] = input.mouse_move_delta();
            if (dx != 0 || dy != 0){

                controller->yaw += (float)-dx * dt * controller->mouseSpeed;
                controller->pitch += (float)-dy * dt * controller->mouseSpeed;

                glm::quat yaw = glm::angleAxis(glm::radians(controller->yaw), glm::vec3(0, 1, 0));
                glm::quat pitch = glm::angleAxis(glm::radians(controller->pitch), glm::vec3(1, 0, 0));
                rot = glm::normalize(pitch * yaw);
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

        auto up = rot * glm::vec3(0, controller->speed * dt, 0);
        if (input.key_down(EG_KEY_E)) {
            pos += up;
        }
        if (input.key_down(EG_KEY_Q)) {
            pos -= up;
        }

        if (position->vec != pos || rotation->quat != rot){
            position->vec = pos;
            rotation->quat = rot;
            m_eventBus->emit(beagle::OnCameraUpdate{camera.owner().id()});
        }

        input.refresh();
    }
}
