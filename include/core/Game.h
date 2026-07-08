#pragma once

#include "controller/CollisionSystem.h"
#include "controller/InputHandler.h"
#include "model/World.h"
#include "service/AudioService.h"
#include "view/Screen.h"

#include <memory>

class Game {
public:
    Game();

    void start();
    void pause();
    void resume();
    void gameLoop();

private:
    World world;
    std::unique_ptr<Screen> currentScreen;
    std::unique_ptr<AudioService> audioService;
    InputHandler inputHandler;
    CollisionSystem collisionSystem;
    bool running;
};
