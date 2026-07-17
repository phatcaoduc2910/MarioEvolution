#include "core/Game.h"

#include "view/PauseScreen.h"
#include "view/StartScreen.h"

Game::Game()
    : currentScreen(std::make_unique<StartScreen>()),
      audioService(std::make_unique<SoundManager>()) {}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::start() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
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
    if (playing && dynamic_cast<PauseScreen*>(currentScreen.get()) != nullptr) {
        currentScreen.reset();
        audioService->play("theme");
    }
}

void Game::gameLoop() {
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                continue;
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
        }

        if (!running) {
            break;
        }

        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderClear(renderer);

        if (currentScreen != nullptr) {
            currentScreen->render(renderer);
        } else if (playing) {
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
            world.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }
}
