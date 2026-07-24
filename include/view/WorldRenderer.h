#pragma once

#include "view/AssetRenderer.h"

class World;

class WorldRenderer {
public:
    void render(SDL_Renderer* renderer, SDL_Texture* worldTiles, const World& world);

private:
    AssetRenderer assetRenderer;
};
