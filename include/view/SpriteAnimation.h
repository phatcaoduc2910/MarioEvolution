#pragma once

#include <SDL2/SDL.h>

struct SpriteSheetLayout {
    int originX;
    int originY;
    int frameWidth;
    int frameHeight;
    int strideX;
    int strideY;
};

/**
 * Quản lý frame hiện tại của một dãy animation nằm ngang trên spritesheet.
 */
class SpriteAnimation {
public:
    SpriteAnimation(SpriteSheetLayout layout, int row,
                    int startCol, int frameCount, int frameDurationMs);
    void update(int deltaMs);
    void reset();
    SDL_Rect getCurrentFrame() const;

private:
    SpriteSheetLayout layout;
    int row;
    int startCol;
    int frameCount;
    int frameDurationMs;
    int elapsedMs;
    int currentFrame;
};
