#pragma once

#include "view/AssetRenderer.h"
#include "view/SpriteAnimation.h"
#include "view/TextureManager.h"

#include <SDL2/SDL.h>
#include <cstddef>
#include <vector>

class EffectManager {
public:
    void spawnSmoke(double centerX, double bottomY);
    void spawnImpact(double centerX, double centerY);

    void update(int deltaMs);
    void render(SDL_Renderer* renderer, const TextureManager& textures,
                int offsetX, int offsetY);
    void clear();
    std::size_t activeCount() const;

private:
    struct Effect {
        SpriteAnimation animation;
        double centerX;
        double bottomY;
        double scale;
        int elapsedMs;
        int lifetimeMs;
    };

    void spawn(Effect effect);

    AssetRenderer assetRenderer;
    std::vector<Effect> effects;
};
