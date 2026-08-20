#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>

class HudRenderer {
public:
    void render(SDL_Renderer* renderer, int score, int remainingCoins,
                int timeRemaining, int lives) const;
    // Thanh máu boss: chỉ hiển thị trạng thái, không quyết định gameplay.
    void renderBossHealth(SDL_Renderer* renderer, int currentHp, int maxHp,
                          BossPhase phase) const;
};
