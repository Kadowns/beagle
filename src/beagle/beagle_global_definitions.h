//
// Created by Ricardo on 3/25/2021.
//

#ifndef BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H
#define BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H

#include <eagle/core_global_definitions.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

namespace beagle {

template<typename T>
struct TypeWrapper {
    using type = T;
};

template<typename... Ts, typename TF>
void for_types(TF&& f) {
    (f(TypeWrapper<Ts>{}), ...);
}

}

#endif //BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H
