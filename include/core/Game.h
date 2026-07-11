#pragma once

#include "SDL2/SDL.h"
#include "controller/CollisionSystem.h"
#include "controller/InputHandler.h"
#include "model/World.h"
#include "service/AudioService.h"
#include "view/Screen.h"

#include <memory>

class Game {
public:
    Game();
    ~Game();

    bool start();
    void pause();
    void resume();
    void gameLoop();

private:
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

    World world;
    std::unique_ptr<Screen> currentScreen;
    std::unique_ptr<AudioService> audioService;
    InputHandler inputHandler;
    CollisionSystem collisionSystem;
    bool running{false};
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
};
