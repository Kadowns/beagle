//
// Created by Ricardo on 4/24/2021.
//

#ifndef BEAGLE_TRANSFORM_SYSTEM_H
#define BEAGLE_TRANSFORM_SYSTEM_H

#include <beagle/ecs/system_manager.h>
#include <beagle/ecs/job_manager.h>
#include <beagle/ecs/entity.h>
#include <beagle/ecs/components/transform.h>

namespace beagle {

class TransformUpdateMatricesJob : public BaseJob {
public:
    explicit TransformUpdateMatricesJob(EntityManager* manager);
    void execute() override;

private:
    EntityGroup<Transform> m_transformGroup;
};


struct TransformSystem : BaseSystem {
    void configure(Engine* engine) override;
    JobManager::JobHandle updateMatricesJob;
};


}

#endif //TEMPLATE_APP_TRANSFORM_SYSTEM_H
