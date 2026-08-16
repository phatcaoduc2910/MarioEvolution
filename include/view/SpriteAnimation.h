#pragma once

#include <cstddef>
#include <string>
#include <vector>

class SpriteAnimation {
public:
    SpriteAnimation(std::vector<std::string> frameIds, int frameDurationMs);
    void update(int deltaMs);
    void reset();
    const std::string& getCurrentFrameId() const;

private:
    std::vector<std::string> frameIds;
    int frameDurationMs;
    int elapsedMs;
    std::size_t currentFrame;
};
