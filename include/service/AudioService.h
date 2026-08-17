#pragma once

#include <SDL2/SDL_mixer.h>

#include <map>
#include <string>

// Giao diện điều khiển phát, tạm dừng và âm lượng audio.
class AudioService {
public:
    virtual ~AudioService() = default;

    virtual bool load(const std::string& track, const std::string& path) = 0;

    // Phát hoặc tiếp tục một track.
    virtual void play(const std::string& track) = 0;

    // Tạm dừng một track.
    virtual void pause(const std::string& track) = 0;

    // Đặt âm lượng tổng.
    virtual void setVolume(int volume) = 0;
};

/**
Theo dõi trạng thái audio trong bộ nhớ cho luồng game hiện tại.
Lớp chưa phát âm thanh thật; nó là implementation tối thiểu để Game không phụ thuộc trực tiếp vào một thư viện mixer.
 */
class SoundManager : public AudioService {
public:
    ~SoundManager() override;

    bool load(const std::string& track, const std::string& path) override;
    void play(const std::string& track) override;
    void pause(const std::string& track) override;
    void setVolume(int volume) override;

private:
    bool openAudio();

    int volume{100};
    bool audioOpened{false};
    bool ownsAudioSubsystem{false};
    std::map<std::string, Mix_Chunk*> sounds;
    std::map<std::string, int> channels;
};
