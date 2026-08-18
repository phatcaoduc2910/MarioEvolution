#pragma once

#include <SDL2/SDL.h>

class HudRenderer {
public:
    void render(SDL_Renderer* renderer, int score, int remainingCoins,
                int timeRemaining, int lives) const;
};
