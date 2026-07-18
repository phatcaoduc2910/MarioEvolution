#include "core/Game.h"

/**
 * Khởi tạo Game và chạy vòng lặp chính nếu tài nguyên nạp thành công.
 *
 * @return 0 khi game kết thúc bình thường, 1 khi khởi tạo thất bại.
 */
int main() {
    Game game;

    if (!game.start()) {
        return 1;
    }

    game.gameLoop();

    return 0;
}
