#include "view/SpriteAnimation.h"

#include <utility>

SpriteAnimation::SpriteAnimation(std::vector<std::string> frameIds,
                                 int frameDurationMs)
    : frameIds(std::move(frameIds)),
      frameDurationMs(frameDurationMs),
      elapsedMs(0),
      currentFrame(0) {}

void SpriteAnimation::update(int deltaMs) {
    if (frameIds.size() <= 1 || frameDurationMs <= 0) {
        return;
    }

    elapsedMs += deltaMs;

    // Dùng while để không mất frame khi một nhịp render bị khựng.
    while (elapsedMs >= frameDurationMs) {
        elapsedMs -= frameDurationMs;
        currentFrame = (currentFrame + 1) % frameIds.size();
    }
}

void SpriteAnimation::reset() {
    elapsedMs = 0;
    currentFrame = 0;
}

const std::string& SpriteAnimation::getCurrentFrameId() const {
    static const std::string empty;
    if (currentFrame >= frameIds.size()) {
        return empty;
    }
    return frameIds[currentFrame];
}
