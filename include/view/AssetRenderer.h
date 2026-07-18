#pragma once

#include <SDL2/SDL.h>

/**
 * Bao mỏng quanh SDL_RenderCopyEx để kiểm tra input render dùng chung.
 */
class AssetRenderer {
public:
    AssetRenderer() = default;
    ~AssetRenderer() = default;

    void render(SDL_Renderer* renderer, SDL_Texture* texture,
                const SDL_Rect* srcRect, const SDL_Rect* dstRect,
                SDL_RendererFlip flip = SDL_FLIP_NONE);

    void render(SDL_Renderer* renderer, SDL_Texture* texture,
                int x, int y, int width = 0, int height = 0);
};
