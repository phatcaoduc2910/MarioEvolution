#include "view/AssetRenderer.h"

void AssetRenderer::render(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect, SDL_RendererFlip flip) {
    if (renderer == nullptr || texture == nullptr || dstRect == nullptr) { return; }
    if (dstRect->w <= 0 || dstRect->h <= 0) { return; }
    SDL_RenderCopyEx(renderer, texture, srcRect, dstRect, 0, nullptr, flip);
}

void AssetRenderer::render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height) {
    if (!(renderer && texture)) { return; }
    if (width <= 0 || height <= 0) {
        int texW, texH;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) < 0) { return; }
        if (width <= 0) { width = texW; }
        if (height <= 0) { height = texH; }
    }
    if (width <= 0 || height <= 0) { return; }
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}
