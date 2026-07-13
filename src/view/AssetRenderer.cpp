#include "view/AssetRenderer.h"

/**
 * Vẽ một vùng texture vào hình chữ nhật đích.
 *
 * Bỏ qua renderer, texture hoặc con trỏ hình chữ nhật đích không hợp lệ.
 *
 * @param renderer SDL renderer dùng để vẽ.
 * @param texture Texture cần vẽ.
 * @param srcRect Hình chữ nhật nguồn tùy chọn; nullptr sẽ vẽ toàn bộ texture.
 * @param dstRect Hình chữ nhật đích theo tọa độ màn hình.
 * @param flip Chế độ lật áp dụng khi vẽ.
 */
void AssetRenderer::render(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect, SDL_RendererFlip flip) {
    if (renderer == nullptr || texture == nullptr || dstRect == nullptr) { return; }
    SDL_RenderCopyEx(renderer, texture, srcRect, dstRect, 0, nullptr, flip);
}

/**
 * Vẽ texture tại một vị trí với kích thước đích tùy chọn.
 *
 * Kích thước bị thiếu hoặc không dương sẽ dùng kích thước gốc của texture.
 *
 * @param renderer SDL renderer dùng để vẽ.
 * @param texture Texture cần vẽ.
 * @param x Tọa độ x đích.
 * @param y Tọa độ y đích.
 * @param width Chiều rộng đích; dùng chiều rộng texture khi không dương.
 * @param height Chiều cao đích; dùng chiều cao texture khi không dương.
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
