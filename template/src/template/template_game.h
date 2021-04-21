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

class TemplateGame : public beagle::Game {
public:
    TemplateGame();
    ~TemplateGame() override = default;

    void init(beagle::Engine* engine) override;

    void step(beagle::Engine* engine) override;

    void destroy(beagle::Engine* engine) override;
private:

    beagle::EntityGroup<beagle::Transform, Oscilator> m_oscilatorGroup;
    beagle::EntityGroup<beagle::Transform, Scaler> m_scalerGroup;
};


#endif //TEMPLATE_APP_TEMPLATE_GAME_PLUGIN_H
