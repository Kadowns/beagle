//
// Created by Ricardo on 3/25/2021.
//

#ifndef BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H
#define BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H

namespace beagle {

template<typename T>
struct type_wrapper {
    using type = T;
};

template<typename... Ts, typename TF>
void for_types(TF&& f) {
    (f(type_wrapper<Ts>{}), ...);
}

}

#endif //BEAGLE_BEAGLE_GLOBAL_DEFINITIONS_H
