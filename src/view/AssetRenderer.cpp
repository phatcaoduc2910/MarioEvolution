#include "view/AssetRenderer.h"

/**
 * Renders a texture region into a destination rectangle.
 *
 * Invalid renderer, texture, or destination rectangle pointers are ignored.
 *
 * @param renderer SDL renderer used for drawing.
 * @param texture Texture to draw.
 * @param srcRect Optional source rectangle; nullptr renders the whole texture.
 * @param dstRect Destination rectangle in screen coordinates.
 * @param flip Flip mode applied during rendering.
 */
void AssetRenderer::render(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect, SDL_RendererFlip flip) {
    if (renderer == nullptr || texture == nullptr || dstRect == nullptr) { return; }
    SDL_RenderCopyEx(renderer, texture, srcRect, dstRect, 0, nullptr, flip);
}

/**
 * Renders a texture at a position with optional destination size.
 *
 * Missing or non-positive dimensions fall back to the texture's native size.
 *
 * @param renderer SDL renderer used for drawing.
 * @param texture Texture to draw.
 * @param x Destination x coordinate.
 * @param y Destination y coordinate.
 * @param width Destination width; uses texture width when non-positive.
 * @param height Destination height; uses texture height when non-positive.
 */
void AssetRenderer::render(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height) {
    if (!(renderer && texture)) { return; }
    if (width <= 0 || height <= 0) {
        int texW, texH;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) < 0) { return; }
        if (width <= 0) { width = texW; }
        if (height <= 0) { height = texH; }
    }
    SDL_Rect dstRect = {x, y, width, height};
    SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, 0, nullptr, SDL_FLIP_NONE);
}
