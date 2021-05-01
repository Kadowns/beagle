//
// Created by Ricardo on 4/25/2021.
//

#ifndef BEAGLE_CAMERA_EVENTS_H
#define BEAGLE_CAMERA_EVENTS_H

#include <beagle/ecs/entity.h>

namespace beagle {

struct OnCameraUpdate {
    Entity::Id entityId;
};

}

#endif //TEMPLATE_APP_CAMERA_EVENTS_H
