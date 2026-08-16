#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>

class TerminalScreen {
public:
    void render(SDL_Renderer* renderer, GameState state, int score) const;
};
