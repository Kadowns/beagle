//
// Created by Ricardo on 4/24/2021.
//

#include "transform_system.h"

using namespace beagle;

TransformUpdateMatricesJob::TransformUpdateMatricesJob(EntityManager* manager) : BaseJob("TransformUpdateMatricesJob"){
    m_transformGroup.attach(manager);
}

void TransformUpdateMatricesJob::execute() {
    for (auto[tr] : m_transformGroup) {
        auto entity = tr.owner();
        glm::mat4 matrix(1);
        if (entity.has_component<Position>()){
            matrix = glm::translate(matrix, entity.component<Position>()->vec);
        }
        if (entity.has_component<Rotation>()){
            matrix *= glm::mat4_cast(entity.component<Rotation>()->quat);
        }
        if (entity.has_component<Scale>()){
            matrix = glm::scale(matrix, entity.component<Scale>()->vec);
        }
        tr->matrix = matrix;
        tr->inverseMatrix = glm::inverse(matrix);
    }
}
