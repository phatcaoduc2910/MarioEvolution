#pragma once

#include <SDL2/SDL.h>

/**
 * Quản lý frame hiện tại của một dãy animation nằm ngang trên spritesheet.
 */
class SpriteAnimation {
public:
    SpriteAnimation(int frameWidth, int frameHeight, int row,
                    int startCol, int frameCount, int frameDurationMs);
    void update(int deltaMs);
    void reset();
    SDL_Rect getCurrentFrame() const;

private:
    int frameWidth;
    int frameHeight;
    int row;
    int startCol;
    int frameCount;
    int frameDurationMs;
    int elapsedMs;
    int currentFrame;
};
