//
// Created by Ricardo on 4/29/2021.
//

#ifndef TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H
#define TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H

#include <beagle/ecs/job_system.h>
#include <beagle/ecs/entity.h>

#include <template/ecs/components/camera_controller.h>
#include <beagle/ecs/components/transform.h>
#include <beagle/ecs/components/camera.h>

#include <eagle/timer.h>

class CameraControlSystem : public beagle::BaseJob {
public:
    explicit CameraControlSystem(beagle::EntityManager* manager, eagle::Timer* timer);
    void execute() override;

private:
    beagle::EntityGroup<beagle::Position, beagle::Rotation, CameraController, beagle::Camera> m_cameraPositionGroup;
    eagle::Timer* m_timer;
    eagle::EventBus* m_eventBus;
};


#endif //TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H
