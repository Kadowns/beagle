//
// Created by Ricardo on 4/24/2021.
//

#ifndef BEAGLE_TRANSFORM_SYSTEM_H
#define BEAGLE_TRANSFORM_SYSTEM_H

#include <beagle/ecs/job_system.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/transform.h>

namespace beagle {

class TransformSystem : public BaseJob {
public:
    explicit TransformSystem(EntityManager* manager);
    void execute() override;

private:
    EntityGroup<Transform> m_transformGroup;
};

}

#endif //TEMPLATE_APP_TRANSFORM_SYSTEM_H
