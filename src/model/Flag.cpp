#include "model/Flag.h"

#include "model/Player.h"

#include <SDL2/SDL.h>

/**
 * Khởi tạo cờ chưa được chiếm với kích thước 32x160.
 *
 * @param x Tọa độ x của cờ.
 * @param y Tọa độ y của cờ.
 */
Flag::Flag(double x, double y)
    : StaticObject(x, y, 32, 160),
      captured(false) {}

/**
 * @return true nếu player đã chiếm cờ; ngược lại là false.
 */
bool Flag::isCaptured() const {
    return captured;
}

/**
 * Đánh dấu cờ đã được chiếm và vô hiệu hóa va chạm rắn.
 *
 * @param player Player chạm vào cờ.
 */
void Flag::onCapture(Player& player) {
    if (captured || !player.isAlive() || player.getState() == PlayerState::Dead) {
        return;
    }

    captured = true;
    solid = false;
    showGameOverMessage();
}

/**
 * Ghi thông báo hoàn thành màn chơi bằng SDL log.
 */
void Flag::showGameOverMessage() {
    SDL_Log("Level completed!");
}
