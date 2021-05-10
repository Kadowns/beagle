//
// Created by Ricardo on 4/3/2021.
//

#ifndef BEAGLE_TRANSFORM_H
#define BEAGLE_TRANSFORM_H

#include <beagle/math.h>

namespace beagle {

struct Position {
    Position() : vec(0) {}
    explicit Position(const glm::vec3& position) : vec(position) {}
    Position(float x, float y, float z) : vec(x, y, z) {}
    glm::vec3 vec;
};

struct Rotation {
    Rotation() : quat(glm::vec3(0)) {}
    explicit Rotation(const glm::vec3& rotation) : quat(rotation) {}
    Rotation(float x, float y, float z) : quat(glm::vec3(x, y, z)) {}
    Rotation(float w, float x, float y, float z) : quat(w, x, y, z) {}
    glm::quat quat;
};

struct Scale {
    Scale() : vec(1) {}
    explicit Scale(float scale) : vec(scale) {}
    explicit Scale(const glm::vec3& scale) : vec(scale) {}
    Scale(float x, float y, float z) : vec(x, y, z) {}
    glm::vec3 vec;
};

struct Transform {
    glm::mat4 matrix;
    glm::mat4 inverseMatrix;
};


}

#endif //BEAGLE_TRANSFORM_H
