#pragma once

#include "SDL2/SDL.h"
#include "controller/CollisionSystem.h"
#include "controller/InputHandler.h"
#include "model/World.h"
#include "service/AudioService.h"
#include "view/ActorRenderer.h"
#include "view/Screen.h"
#include "view/WorldRenderer.h"

#include <memory>

class MapEditorService;

/**
 * Sở hữu tài nguyên SDL và điều phối vòng lặp cấp cao của ứng dụng.
 *
 * Game chuyển đổi giữa menu, gameplay và pause screen; World giữ trạng thái
 * gameplay, còn các renderer chỉ chịu trách nhiệm hiển thị.
 */
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
    std::unique_ptr<MapEditorService> mapEditor;
    InputHandler inputHandler;
    CollisionSystem collisionSystem;
    WorldRenderer worldRenderer;
    ActorRenderer playerRenderer;
    bool playing{false};
    bool running{false};
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    SDL_Texture* worldTiles{nullptr};
    SDL_Texture* playerTexture{nullptr};
    Uint32 lastFrameTicks{0};
};
