//
// Created by Ricardo on 3/25/2021.
//

#ifndef BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H
#define BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H

#include <eagle/core_global_definitions.h>

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
