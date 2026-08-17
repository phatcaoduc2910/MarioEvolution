#pragma once

#include "view/AssetRenderer.h"
#include "view/SpriteAnimation.h"
#include "view/TextureManager.h"

class World;

class WorldRenderer {
public:
    void update(int deltaMs);

    void renderBackground(SDL_Renderer* renderer, const TextureManager& textures,
                          int viewWidth, int viewHeight);
    void render(SDL_Renderer* renderer, const TextureManager& textures,
                const World& world);

private:
    AssetRenderer assetRenderer;
    SpriteAnimation coinAnimation{{"coin.1", "coin.2", "coin.3", "coin.4"}, 120};
    SpriteAnimation flagAnimation{
        {"flag.1", "flag.2", "flag.3", "flag.4", "flag.5"}, 120};
};
