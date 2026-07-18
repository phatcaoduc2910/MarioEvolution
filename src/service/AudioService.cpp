#include "service/AudioService.h"

#include <algorithm>

/**
 * Đánh dấu một track đang phát nếu tên track không rỗng.
 *
 * @param track Tên track cần phát.
 */
void SoundManager::play(const std::string& track) {
    if (!track.empty()) {
        tracks[track] = "playing";
    }
}

/**
 * Chuyển track đã biết sang trạng thái tạm dừng.
 *
 * @param track Tên track cần tạm dừng.
 */
void SoundManager::pause(const std::string& track) {
    const auto found = tracks.find(track);
    if (found != tracks.end()) {
        found->second = "paused";
    }
}

/**
 * Đặt âm lượng tổng trong miền hợp lệ từ 0 đến 100.
 *
 * @param volume Giá trị âm lượng mong muốn.
 */
void SoundManager::setVolume(int volume) {
    this->volume = std::clamp(volume, 0, 100);
}
