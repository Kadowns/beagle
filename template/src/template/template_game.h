//
// Created by Ricardo on 4/21/2021.
//

#ifndef TEMPLATE_GAME_PLUGIN_H
#define TEMPLATE_GAME_PLUGIN_H

#include <beagle/engine.h>

struct Oscilator {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    glm::vec3 anchor;
};

struct Scaler {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    glm::vec3 anchor;
};

struct Rotator {
    glm::vec3 frequency;
};

class TemplateGame : public beagle::Game {
public:
    TemplateGame();
    ~TemplateGame() override = default;

    void init(beagle::Engine* engine) override;

    void step(beagle::Engine* engine) override;

    void destroy(beagle::Engine* engine) override;
private:

    std::weak_ptr<eagle::CommandBuffer> m_commandBuffer;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::VertexBuffer> m_instanceVertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::Shader> m_shader;
    std::weak_ptr<eagle::DescriptorSet> m_descriptorSet;

    beagle::EntityGroup<beagle::Transform> m_quadsGroup;
    beagle::EntityGroup<beagle::Position, Oscilator> m_oscilatorGroup;
    beagle::EntityGroup<beagle::Scale, Scaler> m_scalerGroup;
    beagle::EntityGroup<beagle::Rotation, Rotator> m_rotatorGroup;
};


#endif //TEMPLATE_APP_TEMPLATE_GAME_PLUGIN_H
