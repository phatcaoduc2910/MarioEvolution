#include "service/AudioService.h"

#include <algorithm>

void SoundManager::play(const std::string& track) {
    if (!track.empty()) {
        tracks[track] = "playing";
    }
}

void SoundManager::pause(const std::string& track) {
    const auto found = tracks.find(track);
    if (found != tracks.end()) {
        found->second = "paused";
    }
}

void SoundManager::setVolume(int volume) {
    this->volume = std::clamp(volume, 0, 100);
}
