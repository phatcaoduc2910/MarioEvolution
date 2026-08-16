#include "model/Flag.h"

#include "model/Player.h"

#include <SDL2/SDL.h>

Flag::Flag(double x, double y)
    : StaticObject(x, y, kPoleWidth, kPoleHeight),
      captured(false) {}

bool Flag::isCaptured() const {
    return captured;
}

// Xử lý khi người chơi chạm cờ
void Flag::onCapture(Player& player) {
    // Không xử nếu cờ đã được chiếm hoặc người chơi đã chết
    if (captured || !player.isAlive() || player.getState() == PlayerState::Dead) {
        return;
    }

    captured = true;
    solid = false;              // -> khiến cờ không chặn các Actor
    showGameOverMessage();      // Hàm thông báo hoàn thành màn chơi
}

// Thông báo hoàn thành màn chơi
void Flag::showGameOverMessage() {
    SDL_Log("Level completed!");
}
