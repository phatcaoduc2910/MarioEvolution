#include "core/Game.h"

#include "service/MapEditorService.h"


#include <SDL2/SDL_image.h>
#include <algorithm>

namespace {
constexpr const char* kLevelPath = "assets/maps/level1.map";
constexpr int kMapWidth = 25;
constexpr int kMapHeight = 19;
constexpr int kTileSize = 32;
}

Game::Game()
    : currentGameState(StartMenu),
      audioService(std::make_unique<SoundManager>()) {}

Game::~Game() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(worldTiles);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
//Khởi tạo giá trị ban đầu của game
bool Game::start() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    constexpr int imageFlags = IMG_INIT_PNG;
    if ((IMG_Init(imageFlags) & imageFlags) != imageFlags) {
        SDL_Log("SDL_image initialization failed: %s", IMG_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "MarioEvolution",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == nullptr) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    SDL_Surface* worldSurface = IMG_Load("assets/runtime/world_tiles.png");
    if (worldSurface == nullptr) {
        SDL_Log("World texture loading failed: %s", IMG_GetError());
        return false;
    }

    worldTiles = SDL_CreateTextureFromSurface(renderer, worldSurface);
    SDL_FreeSurface(worldSurface);

    if (worldTiles == nullptr) {
        SDL_Log("World texture creation failed: %s", SDL_GetError());
        return false;
    }

    playerTexture = IMG_LoadTexture(renderer, "assets/runtime/mario_super.png");
    if (playerTexture == nullptr) {
        SDL_Log("Player texture load failed: %s", IMG_GetError());
        return false;
    }

    mapEditor = std::make_unique<MapEditorService>(
        kMapWidth,
        kMapHeight,
        kTileSize,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        kLevelPath);
    world.loadLevel(mapEditor->getLevel());

    lastFrameTicks = SDL_GetTicks();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

void Game::pause() {
    currentGameState = Paused;
    pauseScreen.render(renderer);
    audioService->pause("theme");
}

void Game::resume() {
    currentGameState = Playing;
    audioService->play("theme");
}

void Game::edit() {
    currentGameState = Editing;
    audioService->pause("theme");
}
// Xử lý vòng lặp game
void Game::gameLoop() {
    SDL_Event event;
    Option menuOption{Option::StartGame};

    while (currentGameState != Exit) {
        while (SDL_PollEvent(&event)) {

        Key key = inputHandler.mapKey(event.key.keysym.sym);
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                inputHandler.press(key); 
            } else if (event.type == SDL_KEYUP) {
                inputHandler.release(key);
            }

            if(event.type == SDL_QUIT) {
                currentGameState = Exit;
            }
            switch (currentGameState) {
                case StartMenu:{
                    
                    Option action = inputHandler.getMenuOption(menuOption);
                    if (action == Option::StartGame) {
                        world = World();
                        world.loadLevel(mapEditor->getLevel()); 
                        currentGameState = Playing;
                    } else if (action == Option::ExitGame) {
                        currentGameState = Exit;
                    }
                    break;
                }
                case Playing:{
                    if(inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
                        pause();
                    } else if (inputHandler.isPressed(Key::Jump)) {
                        world.getPlayer().jump();
                    } else if (inputHandler.isPressed(Key::Edit)) {
                        inputHandler.release(Key::Edit);
                        currentGameState = Editing;
                        mapEditor->handleEvent(event);
                    }
                    break;
                }
                case Paused:{
                    if(inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
                        resume();
                    }
                    break;
                }
                case Editing:{
                    //Vào map editor
                    const bool wasEditorEnabled = mapEditor->isEnabled();
                    if (mapEditor->handleEvent(event)) {
                        if (!wasEditorEnabled && mapEditor->isEnabled()) {
                            world.getPlayer().setMoveDirection(0);
                        } else if (wasEditorEnabled && !mapEditor->isEnabled()) {
                            world.loadLevel(mapEditor->getLevel());
                        }
                        continue;
                    }

                    if(inputHandler.isPressed(Key::Edit)) {
                        inputHandler.release(Key::Edit);
                        currentGameState = Playing;
                    }

                    break;
                }
                case LevelComplete:{
                    if (inputHandler.isPressed(Key::Enter) || inputHandler.isPressed(Key::Esc)) {
                        currentGameState = StartMenu;
                    }
                    break;
                }
                case GameOver:{
                    if (inputHandler.isPressed(Key::Enter) || inputHandler.isPressed(Key::Esc)) {
                        currentGameState = StartMenu;
                    }
                    break;
                }
                case Exit:{
                    break;
                }
            }
        }

    constexpr int kFixedStepMs = 11;
    static int accumulatorMs = 0;
    const Uint32 now = SDL_GetTicks();
    const Uint32 elapsed = now - lastFrameTicks;
    const int deltaMs = static_cast<int>(std::min<Uint32>(elapsed, 100));
    lastFrameTicks = now;
    accumulatorMs += deltaMs;


    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    switch (currentGameState) {
        case StartMenu:
            startScreen.setOption(menuOption);
            startScreen.render(renderer);
            break;

        case Paused:
            pauseScreen.render(renderer);
            break;
        case Playing:{
            while (accumulatorMs >= kFixedStepMs) {
                int horizontalInput = 0;
                if (inputHandler.isPressed(Key::Left)) --horizontalInput;
                if (inputHandler.isPressed(Key::Right)) ++horizontalInput;
                world.getPlayer().setMoveDirection(horizontalInput);
                world.update();
                collisionSystem.resolve(world);
                if (world.isGameOver()) {
                    currentGameState = GameOver;
                }
                accumulatorMs -= kFixedStepMs;
            }
            playerRenderer.updatePlayer(world.getPlayer(), deltaMs);
            worldRenderer.render(renderer, worldTiles, world);
            playerRenderer.renderPlayer(renderer, playerTexture, world.getPlayer());
            break;
        }
        case Editing:{
            mapEditor->update();
            mapEditor->render(renderer, worldTiles);

            playerRenderer.updatePlayer(world.getPlayer(), deltaMs);
            const SDL_Rect viewport = mapEditor->getMapViewport();
            SDL_RenderSetClipRect(renderer, &viewport);
            playerRenderer.renderPlayer(
                renderer,
                playerTexture,
                world.getPlayer(),
                -mapEditor->getCameraX(),
                -mapEditor->getCameraY());
            SDL_RenderSetClipRect(renderer, nullptr);
        }
        break;
    }
    SDL_RenderPresent(renderer);
}
}
