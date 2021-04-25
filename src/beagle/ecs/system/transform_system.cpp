//
// Created by Ricardo on 4/24/2021.
//

#include "transform_system.h"

using namespace beagle;

TransformSystem::TransformSystem(EntityManager* manager) {
    m_transformGroup.attach(manager);
}

void TransformSystem::execute() {
    for (auto[tr] : m_transformGroup) {
        auto entity = tr.owner();
        glm::mat4 matrix(1);
        if (entity.has_component<Position>()){
            matrix = glm::translate(matrix, entity.component<Position>()->position);
        }
        if (entity.has_component<Rotation>()){
            matrix *= glm::mat4_cast(entity.component<Rotation>()->rotation);
        }
        if (entity.has_component<Scale>()){
            matrix = glm::scale(matrix, entity.component<Scale>()->scale);
        }
        tr->matrix = matrix;
        tr->inverseMatrix = glm::inverse(matrix);
    }
}
