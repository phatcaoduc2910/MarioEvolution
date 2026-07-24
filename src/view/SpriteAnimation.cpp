#include "view/SpriteAnimation.h"

SpriteAnimation::SpriteAnimation(SpriteSheetLayout layout, int row,
                                 int startCol, int frameCount, int frameDurationMs)
    : layout(layout),
      row(row),
      startCol(startCol),
      frameCount(frameCount),
      frameDurationMs(frameDurationMs),
      elapsedMs(0),
      currentFrame(0) {}

void SpriteAnimation::update(int deltaMs) {
    if (frameCount <= 1 || frameDurationMs <= 0) {
        return;
    }

    elapsedMs += deltaMs;

    // Dùng while để không mất frame khi một nhịp render bị khựng.
    while (elapsedMs >= frameDurationMs) {
        elapsedMs -= frameDurationMs;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void SpriteAnimation::reset() {
    elapsedMs = 0;
    currentFrame = 0;
}

SDL_Rect SpriteAnimation::getCurrentFrame() const {
    return {
        layout.originX + (startCol + currentFrame) * layout.strideX,
        layout.originY + row * layout.strideY,
        layout.frameWidth,
        layout.frameHeight
    };
}
