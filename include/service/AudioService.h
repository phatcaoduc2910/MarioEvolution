#pragma once

#include <map>
#include <string>

/**
 * Giao diện điều khiển phát, tạm dừng và âm lượng audio.
 */
class AudioService {
public:
    virtual ~AudioService() = default;

    /**
     * Phát hoặc tiếp tục một track.
     *
     * @param track Tên track cần phát.
     */
    virtual void play(const std::string& track) = 0;

    /**
     * Tạm dừng một track.
     *
     * @param track Tên track cần tạm dừng.
     */
    virtual void pause(const std::string& track) = 0;

    /**
     * Đặt âm lượng tổng.
     *
     * @param volume Âm lượng mong muốn; implementation giới hạn về 0 đến 100.
     */
    virtual void setVolume(int volume) = 0;
};

/**
 * Theo dõi trạng thái audio trong bộ nhớ cho luồng game hiện tại.
 *
 * Lớp chưa phát âm thanh thật; nó là implementation tối thiểu để Game không
 * phụ thuộc trực tiếp vào một thư viện mixer.
 */
class SoundManager : public AudioService {
public:
    void play(const std::string& track) override;
    void pause(const std::string& track) override;
    void setVolume(int volume) override;

private:
    int volume = 100;
    std::map<std::string, std::string> tracks;
};
