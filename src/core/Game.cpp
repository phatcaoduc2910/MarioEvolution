#include "core/Game.h"

#include "service/MapEditorService.h"


#include <SDL2/SDL_image.h>
#include <algorithm>

namespace {
constexpr const char* kLevelPath = "assets/maps/level_horizontal.map";
constexpr int kMapWidth = 25;
constexpr int kMapHeight = 19;
constexpr int kTileSize = 32;
}

Game::Game()
    : audioService(std::make_unique<SoundManager>()) {}

Game::~Game() {
    mapEditor.reset();
    textureManager.reset();
    audioService.reset();
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

    textureManager = std::make_unique<TextureManager>(renderer);
    if (!textureManager->gameTextureLoad()) {
        SDL_Log("Failed to load game textures.");
        return false;
    }

    audioService->load("jump", "assets/audio/sfx/jump.wav");
    audioService->load("win", "assets/audio/sfx/goal.wav");
    audioService->load("gameover", "assets/audio/sfx/gameover.wav");

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
void Game::startLevel() {
    world = World();
    world.loadLevel(mapEditor->getLevel());
    camera.reset();
    currentGameState = Playing;
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
                        startLevel();
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
                        Player& player = world.getPlayer();
                        const bool canJump = player.isAlive() && player.isOnGround();
                        player.jump();
                        if (canJump) {
                            audioService->play("jump");
                        }
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
                case LevelComplete:
                case GameOver:{
                    if (inputHandler.isPressed(Key::Enter)) {
                        inputHandler.release(Key::Enter);
                        startLevel();
                    } else if (inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
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
    constexpr double kFixedStepSeconds = kFixedStepMs / 1000.0;
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
                world.update(kFixedStepSeconds);
                collisionSystem.update(world, kFixedStepSeconds);
                if (world.isLevelComplete()) {
                    currentGameState = LevelComplete;
                    audioService->play("win");
                } else if (world.isGameOver()) {
                    currentGameState = GameOver;
                    audioService->play("gameover");
                }

                if (currentGameState != Playing) {
                    accumulatorMs = 0;
                    break;
                }
                accumulatorMs -= kFixedStepMs;
            }
            worldRenderer.update(deltaMs);
            actorRenderer.updatePlayer(world.getPlayer(), deltaMs);
            actorRenderer.updateEnemies(deltaMs);

            const LevelData& level = mapEditor->getLevel();
            const int worldWidth = level.getWidth() * level.getTileSize();
            camera.follow(world.getPlayer(), worldWidth);
            const int offsetX = camera.getOffsetX();

            worldRenderer.renderBackground(
                renderer, *textureManager, WINDOW_WIDTH, WINDOW_HEIGHT);
            worldRenderer.render(
                renderer, *textureManager, world, offsetX, 0);
            actorRenderer.renderEnemies(
                renderer, *textureManager, world, offsetX, 0);
            actorRenderer.renderPlayer(
                renderer, *textureManager, world.getPlayer(), offsetX, 0);
            hudRenderer.render(
                renderer, world.getScore(), world.getPlayer().getState());
            break;
        }
        case Editing:{
            mapEditor->update();
            mapEditor->render(renderer, *textureManager);

            actorRenderer.updatePlayer(world.getPlayer(), deltaMs);
            actorRenderer.updateEnemies(deltaMs);
            const SDL_Rect viewport = mapEditor->getMapViewport();
            SDL_RenderSetClipRect(renderer, &viewport);
            actorRenderer.renderEnemies(
                renderer,
                *textureManager,
                world,
                -mapEditor->getCameraX(),
                -mapEditor->getCameraY());
            actorRenderer.renderPlayer(
                renderer,
                *textureManager,
                world.getPlayer(),
                -mapEditor->getCameraX(),
                -mapEditor->getCameraY());
            SDL_RenderSetClipRect(renderer, nullptr);
        }
        break;
        case LevelComplete:
        case GameOver:
            worldRenderer.renderBackground(
                renderer, *textureManager, WINDOW_WIDTH, WINDOW_HEIGHT);
            worldRenderer.render(
                renderer, *textureManager, world, camera.getOffsetX(), 0);
            actorRenderer.renderEnemies(
                renderer, *textureManager, world, camera.getOffsetX(), 0);
            actorRenderer.renderPlayer(
                renderer,
                *textureManager,
                world.getPlayer(),
                camera.getOffsetX(),
                0);
            terminalScreen.render(renderer, currentGameState, world.getScore());
            break;
        case Exit:
            break;
    }
    SDL_RenderPresent(renderer);
}
}
