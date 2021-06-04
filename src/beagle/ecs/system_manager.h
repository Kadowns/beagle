//
// Created by Ricardo on 5/20/2021.
//

#ifndef BEAGLE_SYSTEM_MANAGER_H
#define BEAGLE_SYSTEM_MANAGER_H

#include <vector>
#include <cassert>

#include <eagle/memory/pointer.h>

namespace beagle {

class Engine;
class SystemManager;

struct BaseSystem {
    virtual ~BaseSystem() = default;
    virtual void configure(Engine* engine) = 0;
};

class SystemManager {
public:

    explicit SystemManager(Engine* engine) : m_engine(engine) {}

    template<typename S, typename ...Args>
    S* attach(Args&& ...args);

    template<typename S>
    S* system();

private:

    template<typename T>
    size_t system_index();

private:
    Engine* m_engine;
    std::vector<eagle::StrongPointer<BaseSystem>> m_systems;
    static size_t s_systemIndexCounter;
};

template<typename T>
size_t SystemManager::system_index() {
    static const size_t index = s_systemIndexCounter++;
    return index;
}

template<typename S, typename... Args>
S* SystemManager::attach(Args&& ... args) {
    size_t index = system_index<S>();
    if (index >= m_systems.size()){
        m_systems.resize(index);
    }
    auto system = eagle::make_strong<S>(std::forward<Args>(args)...);
    system->configure(m_engine);
    m_systems.emplace_back(system);
    return system.get();
}

template<typename S>
S* SystemManager::system() {
    size_t index = system_index<S>();
    assert(index < m_systems.size() && "System index out of bounds");
    return m_systems[index].get();
}

}

#endif //BEAGLE_SYSTEM_MANAGER_H
