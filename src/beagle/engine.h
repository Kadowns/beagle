//
// Created by Ricardo on 3/27/2021.
//

#ifndef BEAGLE_ENGINE_H
#define BEAGLE_ENGINE_H

#include <beagle/entity_manager.h>

#include <eagle/application_delegate.h>

#include <eagle/renderer/rendering_context.h>
#include <eagle/renderer/command_buffer.h>

namespace beagle {

struct Transform {
    float position[3];
    float rotation[4];
    float scale[3];
};

struct Rigidbody {
    float velocity[3];
};

class Engine : public eagle::ApplicationDelegate {
public:
    Engine();

    void init() override;

    void step() override;

    void destroy() override;

private:
    eagle::EventBus m_entityEventBus;
    EntityManager m_entityManager;
    EntityGroup<Transform, Rigidbody> m_physicsGroup;
    std::weak_ptr<eagle::CommandBuffer> m_commandBuffer;

};

}

#endif //BEAGLE_APPLICATION_H
