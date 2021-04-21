//
// Created by Ricardo on 4/21/2021.
//

#include "template_game.h"

TemplateGame::TemplateGame() {
    EG_LOG_CREATE("template");
}

void TemplateGame::init(beagle::Engine* engine) {
    EG_TRACE("template", "init called");

    EG_DEBUG("template", "Components family id Transform: {0}, Scaler: {1}, Oscilator:{2}",
             beagle::ComponentHelper<beagle::Transform>::family(),
             beagle::ComponentHelper<Scaler>::family(),
             beagle::ComponentHelper<Oscilator>::family()
             );

    auto e = engine->entities().create();
    e.assign<beagle::Transform>(glm::vec3(0, 0, 0));
    e.assign<Scaler>();

    e = engine->entities().create();
    auto tr = e.assign<beagle::Transform>(glm::vec3(-0.5, 0.5, 0));
    auto osc = e.assign<Oscilator>();
    e.assign<Scaler>();
    osc->anchor = tr->position;

    e = engine->entities().create();
    tr = e.assign<beagle::Transform>(glm::vec3(0.5, -0.5, 0));
    osc = e.assign<Oscilator>();
    osc->frequency = -1;
    osc->anchor = tr->position;

    m_oscilatorGroup.attach(&engine->entities());
    m_scalerGroup.attach(&engine->entities());


    auto oscilatorJob = engine->jobs().add_job([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_oscilatorGroup){
            tr->position.x = osc->anchor.x + sinf(t * osc->frequency) * osc->amplitude;
        }
    });

    auto scalerJob = engine->jobs().add_job([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_scalerGroup){
            auto amplitude = osc->amplitude;
            auto frequency = osc->frequency;
            auto scale = tr->scale;
            scale = glm::vec3(1) * (sinf(frequency * t) * amplitude);
            tr->scale = scale;
        }
    });

    engine->jobs().add_dependency(engine->quad_job_id(), oscilatorJob);
    engine->jobs().add_dependency(engine->quad_job_id(), scalerJob);
}

void TemplateGame::step(beagle::Engine* engine) {
    EG_TRACE("template", "step called");
}

void TemplateGame::destroy(beagle::Engine* engine) {
    EG_TRACE("template", "destroy called");
}

