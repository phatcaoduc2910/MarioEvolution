#pragma once

#include <cstddef>
#include <string>
#include <vector>

class SpriteAnimation {
public:
    SpriteAnimation(std::vector<std::string> frameIds, int frameDurationMs,
                    bool loop = true);
    void update(int deltaMs);
    void reset();
    bool isFinished() const;
    int getTotalDurationMs() const;
    const std::string& getCurrentFrameId() const;

private:
    std::vector<std::string> frameIds;
    int frameDurationMs;
    int elapsedMs;
    std::size_t currentFrame;
    bool loop;
    bool finished;
};
