//
// Created by Ricardo on 4/29/2021.
//

#ifndef TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H
#define TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H


#include <beagle/ecs/entity.h>

#include <template/ecs/components/camera_controller.h>
#include <beagle/ecs/components/transform.h>
#include <beagle/ecs/components/camera.h>

#include <eagle/timer.h>
#include <beagle/ecs/job_graph.h>

class CameraControlJob {
public:
    explicit CameraControlJob(beagle::EntityManager* manager, eagle::Timer* timer);
    beagle::JobResult operator()();

private:
    beagle::EntityGroup<beagle::Position, beagle::Rotation, CameraController, beagle::Camera> m_cameraPositionGroup;
    eagle::Timer* m_timer;
    eagle::EventBus* m_eventBus;
};


#endif //TEMPLATE_APP_CAMERA_CONTROL_SYSTEM_H
