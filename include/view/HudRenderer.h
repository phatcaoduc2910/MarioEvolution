#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>

class HudRenderer {
public:
    void render(SDL_Renderer* renderer, int score, PlayerState state) const;
};
