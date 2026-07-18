#include "view/SpriteAnimation.h"

/**
 * Khởi tạo một dãy frame nằm ngang trên spritesheet.
 *
 * @param layout Gốc, kích thước frame và khoảng cách giữa các frame.
 * @param row Hàng chứa animation trên spritesheet.
 * @param startCol Cột đầu tiên của animation.
 * @param frameCount Số frame trong animation.
 * @param frameDurationMs Thời gian hiển thị mỗi frame, tính bằng mili giây.
 */
SpriteAnimation::SpriteAnimation(SpriteSheetLayout layout, int row,
                                 int startCol, int frameCount, int frameDurationMs)
    : layout(layout),
      row(row),
      startCol(startCol),
      frameCount(frameCount),
      frameDurationMs(frameDurationMs),
      elapsedMs(0),
      currentFrame(0) {}

/**
 * Cập nhật frame hiện tại theo thời gian đã trôi qua.
 *
 * @param deltaMs Thời gian từ frame game trước, tính bằng mili giây.
 */
void SpriteAnimation::update(int deltaMs) {
    if (frameCount <= 1 || frameDurationMs <= 0) {
        return;
    }

    elapsedMs += deltaMs;

    while (elapsedMs >= frameDurationMs) {
        elapsedMs -= frameDurationMs;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

/**
 * Đưa animation về frame đầu tiên và xóa thời gian đang tích lũy.
 */
void SpriteAnimation::reset() {
    elapsedMs = 0;
    currentFrame = 0;
}

/**
 * Tính hình chữ nhật nguồn dùng để cắt frame hiện tại từ spritesheet.
 *
 * Tọa độ x dịch theo cột và currentFrame; tọa độ y chọn hàng animation.
 * SDL_RenderCopyEx sẽ dùng hình chữ nhật này để lấy đúng vùng pixel, nên hàm
 * không tạo hay sao chép thành một ảnh mới.
 *
 * @return SDL_Rect mô tả vùng frame nguồn trên spritesheet.
 */
SDL_Rect SpriteAnimation::getCurrentFrame() const {
    return {
        layout.originX + (startCol + currentFrame) * layout.strideX,
        layout.originY + row * layout.strideY,
        layout.frameWidth,
        layout.frameHeight
    };
}
