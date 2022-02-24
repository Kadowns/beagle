//
// Created by Ricardo on 4/24/2021.
//

#ifndef BEAGLE_TRANSFORM_SYSTEM_H
#define BEAGLE_TRANSFORM_SYSTEM_H


#include <beagle/ecs/entity.h>
#include <beagle/ecs/job_graph.h>
#include <beagle/ecs/components/transform.h>

namespace beagle {

class TransformUpdateMatricesJob {
public:
    explicit TransformUpdateMatricesJob(EntityManager* manager);
    JobResult operator()();

private:
    EntityGroup<Transform> m_transformGroup;
};

}

#endif //TEMPLATE_APP_TRANSFORM_SYSTEM_H
