#pragma once

#include <map>
#include <string>

class AudioService {
public:
    virtual ~AudioService() = default;

    virtual void play(const std::string& track) = 0;

    virtual void pause(const std::string& track) = 0;

    virtual void setVolume(int volume) = 0;
};

class SoundManager : public AudioService {
public:
    void play(const std::string& track) override;
    void pause(const std::string& track) override;
    void setVolume(int volume) override;

private:
    int volume = 100;
    std::map<std::string, std::string> tracks;
};
