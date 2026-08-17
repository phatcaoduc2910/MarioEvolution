#include "service/AudioService.h"

#include <SDL2/SDL.h>

#include <algorithm>

SoundManager::~SoundManager() {
    for (const auto& [track, sound] : sounds) {
        (void)track;
        Mix_FreeChunk(sound);
    }

    if (audioOpened) {
        Mix_CloseAudio();
    }
    if (ownsAudioSubsystem) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

bool SoundManager::openAudio() {
    if (audioOpened) {
        return true;
    }

    if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            SDL_Log("Audio initialization failed: %s", SDL_GetError());
            return false;
        }
        ownsAudioSubsystem = true;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        SDL_Log("Audio device open failed: %s", Mix_GetError());
        if (ownsAudioSubsystem) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            ownsAudioSubsystem = false;
        }
        return false;
    }

    audioOpened = true;
    return true;
}

bool SoundManager::load(const std::string& track, const std::string& path) {
    if (track.empty() || path.empty() || !openAudio()) {
        return false;
    }

    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (sound == nullptr) {
        SDL_Log("Sound load failed (%s): %s", path.c_str(), Mix_GetError());
        return false;
    }

    const auto existing = sounds.find(track);
    if (existing != sounds.end()) {
        Mix_FreeChunk(existing->second);
    }
    sounds[track] = sound;
    Mix_VolumeChunk(sound, volume * MIX_MAX_VOLUME / 100);
    return true;
}

// Đánh dấu một track đang phát nếu tên track không rỗng.
void SoundManager::play(const std::string& track) {
    const auto found = sounds.find(track);
    if (found == sounds.end()) {
        return;
    }

    const int channel = Mix_PlayChannel(-1, found->second, 0);
    if (channel < 0) {
        SDL_Log("Sound playback failed (%s): %s", track.c_str(), Mix_GetError());
    } else {
        channels[track] = channel;
    }
}

// Chuyển track đã biết sang trạng thái tạm dừng.
void SoundManager::pause(const std::string& track) {
    const auto found = channels.find(track);
    if (found != channels.end() && Mix_Playing(found->second) != 0) {
        Mix_Pause(found->second);
    }
}

// Đặt âm lượng tổng trong miền hợp lệ từ 0 đến 100.
void SoundManager::setVolume(int volume) {
    this->volume = std::clamp(volume, 0, 100);
    const int mixerVolume = this->volume * MIX_MAX_VOLUME / 100;
    for (const auto& [track, sound] : sounds) {
        (void)track;
        Mix_VolumeChunk(sound, mixerVolume);
    }
}
