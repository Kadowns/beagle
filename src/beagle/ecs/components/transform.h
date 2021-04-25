//
// Created by Ricardo on 4/3/2021.
//

#ifndef BEAGLE_TRANSFORM_H
#define BEAGLE_TRANSFORM_H

#include <beagle/beagle_global_definitions.h>

namespace beagle {

struct Position {
    Position() : position(0) {}
    explicit Position(const glm::vec3& position) : position(position) {}
    Position(float x, float y, float z) : position(x, y, z) {}
    glm::vec3 position;
};

struct Rotation {
    Rotation() : rotation(glm::vec3(0)) {}
    explicit Rotation(const glm::vec3& rotation) : rotation(rotation) {}
    Rotation(float x, float y, float z) : rotation(glm::vec3(x, y, z)) {}
    Rotation(float x, float y, float z, float w) : rotation(x, y, z, w) {}
    glm::quat rotation;
};

struct Scale {
    Scale() : scale(1) {}
    explicit Scale(const glm::vec3& scale) : scale(scale) {}
    Scale(float x, float y, float z) : scale(x, y, z) {}
    glm::vec3 scale;
};

struct Transform {
    glm::mat4 matrix;
    glm::mat4 inverseMatrix;
};


}

#endif //BEAGLE_TRANSFORM_H
