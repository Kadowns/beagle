//
// Created by Ricardo on 5/6/2021.
//

#ifndef BEAGLE_LIGHT_H
#define BEAGLE_LIGHT_H

#include <beagle/math.h>

namespace beagle {

struct DirectionalLight {
    explicit DirectionalLight(const glm::vec3& color = glm::vec3(1), float intensity = 1.0f) :
    color(color), intensity(intensity) {}
    glm::vec3 color;
    float intensity;
};

struct PointLight {
    explicit PointLight(const glm::vec3& color = glm::vec3(1), float intensity = 1.0f) :
    color(color), intensity(intensity){}
    glm::vec3 color;
    float intensity;
};

}

#endif //BEAGLE_LIGHT_H
