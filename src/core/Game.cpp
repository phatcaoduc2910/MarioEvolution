#include "core/Game.h"

Game::Game()
    : currentScreen(std::make_unique<StartScreen>()),
      audioService(std::make_unique<SoundManager>()),
      running(false) {}

void Game::start() {}

void Game::pause() {}

void Game::resume() {}

void Game::gameLoop() {}
