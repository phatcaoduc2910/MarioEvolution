#pragma once

#include <SDL2/SDL_mixer.h>

#include <map>
#include <string>

// Giao diện điều khiển phát, tạm dừng và âm lượng audio.
class AudioService {
public:
    virtual ~AudioService() = default;

    // Nạp một sound vào bộ nhớ và ánh xạ bằng track key.
    virtual bool load(const std::string& track, const std::string& path) = 0;

    // Phát hoặc tiếp tục một track.
    virtual void play(const std::string& track, bool loop = false) = 0;

    // Tạm dừng một track.
    virtual void pause(const std::string& track) = 0;

    // Đặt âm lượng tổng.
    virtual void setVolume(int volume) = 0;
    virtual void setMusicMuted(bool muted) = 0;
    virtual void setSfxMuted(bool muted) = 0;
    virtual bool isMusicMuted() const = 0;
    virtual bool isSfxMuted() const = 0;
};

/**
Theo dõi trạng thái audio trong bộ nhớ cho luồng game hiện tại.
Lớp chưa phát âm thanh thật; nó là implementation tối thiểu để Game không phụ thuộc trực tiếp vào một thư viện mixer.
 */
class SoundManager : public AudioService {
public:
    ~SoundManager() override;

    bool load(const std::string& track, const std::string& path) override;
    void play(const std::string& track, bool loop) override;
    void pause(const std::string& track) override;
    void setVolume(int volume) override;
    void setMusicMuted(bool muted) override;
    void setSfxMuted(bool muted) override;
    bool isMusicMuted() const override;
    bool isSfxMuted() const override;

private:
    bool openAudio();

    int volume{100};
    bool audioOpened{false};
    bool ownsAudioSubsystem{false};
    bool musicMuted{false};
    bool sfxMuted{false};
    std::map<std::string, Mix_Chunk*> sounds;
    std::map<std::string, int> channels;
};
