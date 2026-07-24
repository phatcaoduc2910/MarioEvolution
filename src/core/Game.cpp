#include "core/Game.h"

#include "service/MapEditorService.h"
#include "view/PauseScreen.h"
#include "view/StartScreen.h"

#include <SDL2/SDL_image.h>
#include <algorithm>

namespace {
constexpr const char* kLevelPath = "assets/level1.map";
constexpr int kMapWidth = 25;
constexpr int kMapHeight = 19;
constexpr int kTileSize = 32;
}

Game::Game()
    : currentScreen(std::make_unique<StartScreen>()),
      audioService(std::make_unique<SoundManager>()) {}

Game::~Game() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(worldTiles);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

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

    SDL_Surface* worldSurface = IMG_Load("assets/WorldTiles.png");
    if (worldSurface == nullptr) {
        SDL_Log("World texture loading failed: %s", IMG_GetError());
        return false;
    }

    SDL_SetColorKey(
        worldSurface,
        SDL_TRUE,
        SDL_MapRGB(worldSurface->format, 0, 0, 0)
    );
    worldTiles = SDL_CreateTextureFromSurface(renderer, worldSurface);
    SDL_FreeSurface(worldSurface);

    if (worldTiles == nullptr) {
        SDL_Log("World texture creation failed: %s", SDL_GetError());
        return false;
    }

    playerTexture = IMG_LoadTexture(renderer, "assets/Player.png");
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
    running = true;
    return true;
}

void Game::pause() {
    if (playing && currentScreen == nullptr) {
        currentScreen = std::make_unique<PauseScreen>();
        audioService->pause("theme");
    }
}

void Game::resume() {
    if (playing && currentScreen != nullptr) {
        currentScreen.reset();
        audioService->play("theme");
    }
}

void Game::gameLoop() {
    SDL_Event event;

    while (running) {
        // Hút hết event trước khi update để input không trễ sang frame sau.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (playing && currentScreen == nullptr && mapEditor != nullptr) {
                const bool wasEditorEnabled = mapEditor->isEnabled();
                if (mapEditor->handleEvent(event)) {
                    if (!wasEditorEnabled && mapEditor->isEnabled()) {
                        world.getPlayer().setMoveDirection(0);
                    } else if (wasEditorEnabled && !mapEditor->isEnabled()) {
                        world.loadLevel(mapEditor->getLevel());
                    }
                    continue;
                }
            }

            if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
                continue;
            }

            Key key = Key::Enter;
            if (!inputHandler.mapKey(event.key.keysym.sym, key)) {
                continue;
            }

            if (event.type == SDL_KEYUP) {
                inputHandler.release(key);
                continue;
            }

            // Menu và pause chỉ phản ứng một lần cho mỗi lần nhấn.
            if (event.key.repeat != 0) {
                continue;
            }

            inputHandler.press(key);

            if (auto* startScreen = dynamic_cast<StartScreen*>(currentScreen.get())) {
                const ScreenAction action = startScreen->handleInput(inputHandler);

                if (action == ScreenAction::StartGame) {
                    playing = true;
                    currentScreen.reset();
                    audioService->play("theme");
                } else if (action == ScreenAction::ExitGame) {
                    running = false;
                }
            } else if (auto* pauseScreen = dynamic_cast<PauseScreen*>(currentScreen.get())) {
                const ScreenAction action = pauseScreen->handleInput(inputHandler);

                if (action == ScreenAction::ResumeGame) {
                    resume();
                }
            } else if (playing) {
                if (key == Key::Pause) {
                    pause();
                } else if (key == Key::Jump) {
                    world.getPlayer().jump();
                }
            }

            if (!running) {
                break;
            }
        }

        if (!running) {
            break;
        }

        const Uint32 now = SDL_GetTicks();
        const Uint32 elapsed = now - lastFrameTicks;
        const int deltaMs = static_cast<int>(std::min<Uint32>(elapsed, 100));
        lastFrameTicks = now;

        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderClear(renderer);

        // Có screen phủ thì World đứng yên.
        if (currentScreen != nullptr) {
            currentScreen->render(renderer);
        } else if (playing) {
            if (mapEditor != nullptr && mapEditor->isEnabled()) {
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
            } else {
                int horizontalInput = 0;
                if (inputHandler.isPressed(Key::Left)) {
                    --horizontalInput;
                }
                if (inputHandler.isPressed(Key::Right)) {
                    ++horizontalInput;
                }
                world.getPlayer().setMoveDirection(horizontalInput);

                world.update();
                collisionSystem.resolve(world);

                playerRenderer.updatePlayer(world.getPlayer(), deltaMs);

                worldRenderer.render(renderer, worldTiles, world);
                playerRenderer.renderPlayer(
                    renderer,
                    playerTexture,
                    world.getPlayer());
            }
        }

        SDL_RenderPresent(renderer);
    }
}
