#pragma once

#include <SDL2/SDL.h>

class InputHandler;

enum class ScreenAction {
    None,
    StartGame,
    ExitGame,
    ResumeGame
};

class Screen {
public:
    virtual ~Screen() = default;

    virtual void render(SDL_Renderer* renderer) const = 0;

    virtual ScreenAction handleInput(InputHandler& input) = 0;
};
