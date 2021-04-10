//
// Created by Ricardo on 4/3/2021.
//

#ifndef BEAGLE_TRANSFORM_H
#define BEAGLE_TRANSFORM_H

#include <beagle/beagle_global_definitions.h>

namespace beagle {

struct Transform {
    Transform(
            const glm::vec3& position = glm::vec3(0),
            const glm::vec3& rotation = glm::vec3(0),
            const glm::vec3& scale = glm::vec3(0)) :
            position(position), rotation(rotation), scale(scale) {}

    inline glm::vec3 front()    const { return rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    inline glm::vec3 up()       const { return rotation * glm::vec3(0.0f, 1.0f, 0.0f);  }
    inline glm::vec3 right()    const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f);  }
    inline glm::mat4 matrix()   const { return glm::scale(glm::translate(glm::mat4(1), position) * glm::mat4_cast(rotation), scale);}

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

};

}

#endif //BEAGLE_TRANSFORM_H
