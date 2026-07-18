#pragma once

#include "view/AssetRenderer.h"

class World;

/**
 * Chuyển loại thực thể trong World thành sprite trong WorldTiles.png.
 */
class WorldRenderer {
public:
    void render(SDL_Renderer* renderer, SDL_Texture* worldTiles, const World& world);

private:
    AssetRenderer assetRenderer;
};
