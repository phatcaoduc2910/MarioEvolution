#include "view/SpriteAnimation.h"

#include <utility>

SpriteAnimation::SpriteAnimation(std::vector<std::string> frameIds,
                                 int frameDurationMs,
                                 bool loop)
    : frameIds(std::move(frameIds)),
      frameDurationMs(frameDurationMs),
      elapsedMs(0),
      currentFrame(0),
      loop(loop),
      finished(false) {}

void SpriteAnimation::update(int deltaMs) {
    if (frameIds.empty() || frameDurationMs <= 0 || finished) {
        return;
    }

    elapsedMs += deltaMs;

    // Dùng while để không mất frame khi một nhịp render bị khựng.
    while (elapsedMs >= frameDurationMs) {
        elapsedMs -= frameDurationMs;

        if (currentFrame + 1 < frameIds.size()) {
            ++currentFrame;
        } else if (loop) {
            currentFrame = 0;
        } else {
            finished = true;
            elapsedMs = 0;
            break;
        }
    }
}

void SpriteAnimation::reset() {
    elapsedMs = 0;
    currentFrame = 0;
    finished = false;
}

bool SpriteAnimation::isFinished() const {
    return finished;
}

int SpriteAnimation::getTotalDurationMs() const {
    return static_cast<int>(frameIds.size()) * frameDurationMs;
}

const std::string& SpriteAnimation::getCurrentFrameId() const {
    static const std::string empty;
    if (currentFrame >= frameIds.size()) {
        return empty;
    }
    return frameIds[currentFrame];
}
