//
// Created by Ricardo on 4/21/2021.
//

#ifndef BEAGLE_GAME_PLUGIN_H
#define BEAGLE_GAME_PLUGIN_H

namespace beagle {

class Engine;

class Game {
public:
    virtual ~Game() = default;
    virtual void init(Engine* engine) = 0;
    virtual void step(Engine* engine) = 0;
    virtual void destroy(Engine* engine) = 0;
};

}

#endif //BEAGLE_GAME_PLUGIN_H
