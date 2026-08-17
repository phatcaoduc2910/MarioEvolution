#pragma once

#include "SDL2/SDL.h"
#include "controller/CollisionSystem.h"
#include "controller/InputHandler.h"
#include "model/World.h"
#include "service/AudioService.h"
#include "view/ActorRenderer.h"
#include "core/Types.h"
#include "view/HudRenderer.h"
#include "view/Screen.h"
#include "view/PauseScreen.h"
#include "view/StartScreen.h"
#include "view/TerminalScreen.h"
#include "view/WorldRenderer.h"
#include "view/TextureManager.h"

#include <memory>

class MapEditorService;

class Game {
public:
    Game();
    ~Game();

    bool start();
    void pause();
    void resume();
    void edit();
    void gameLoop();

private:
    void startLevel();

    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

    World world;
    std::unique_ptr<AudioService> audioService;
    std::unique_ptr<MapEditorService> mapEditor;
    InputHandler inputHandler;
    CollisionSystem collisionSystem;
    WorldRenderer worldRenderer;
    ActorRenderer actorRenderer;
    HudRenderer hudRenderer;
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    Uint32 lastFrameTicks{0};
    GameState currentGameState{StartMenu};
    PauseScreen pauseScreen;
    StartScreen startScreen;
    std::unique_ptr<TextureManager> textureManager;
    TerminalScreen terminalScreen;
};
