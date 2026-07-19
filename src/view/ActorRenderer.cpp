#include "view/ActorRenderer.h"
#include "model/Player.h"

#include <cmath>

namespace {
constexpr SpriteSheetLayout kPlayerSpriteLayout{
    0, 16, 14, 32, 16, 32
};
constexpr int kPlayerRenderScale = 2;
}

/**
 * Khởi tạo các dãy animation của small player trên spritesheet.
 */
ActorRenderer::ActorRenderer()
    : idleAnimation(kPlayerSpriteLayout, 0, 0, 8, 160),
      walkAnimation(kPlayerSpriteLayout, 2, 0, 4, 100),
      jumpAnimation(kPlayerSpriteLayout, 1, 0, 4, 120),
      currentState(PlayerAnimationState::Idle) {}

/**
 * Chọn trạng thái animation từ trạng thái vật lý của player.
 *
 * @param player Player cần kiểm tra.
 * @return Trạng thái Jump, Walk hoặc Idle phù hợp.
 */
ActorRenderer::PlayerAnimationState
ActorRenderer::selectState(const Player& player) const {
    constexpr double kMotionEpsilon = 0.001;

    if (!player.isOnGround()) {
        return PlayerAnimationState::Jump;
    }

    if (std::abs(player.getVelocityX()) > kMotionEpsilon) {
        return PlayerAnimationState::Walk;
    }

    return PlayerAnimationState::Idle;
}

/**
 * Lấy bộ đếm animation ứng với trạng thái được chọn.
 *
 * @param state Trạng thái animation của player.
 * @return Tham chiếu tới SpriteAnimation tương ứng.
 */
SpriteAnimation& ActorRenderer::animationFor(PlayerAnimationState state) {
    switch (state) {
        case PlayerAnimationState::Walk:
            return walkAnimation;
        case PlayerAnimationState::Jump:
            return jumpAnimation;
        case PlayerAnimationState::Idle:
        default:
            return idleAnimation;
    }
}

/**
 * Cập nhật frame animation của player và reset khi trạng thái thay đổi.
 *
 * @param player Player cung cấp vận tốc và trạng thái mặt đất.
 * @param deltaMs Thời gian từ frame game trước, tính bằng mili giây.
 */
void ActorRenderer::updatePlayer(const Player& player, int deltaMs) {
    const PlayerAnimationState nextState = selectState(player);

    if (nextState != currentState) {
        currentState = nextState;
        animationFor(currentState).reset();
    }

    animationFor(currentState).update(deltaMs);
}

/**
 * Cắt frame hiện tại từ texture spritesheet và vẽ player lên màn hình.
 *
 * getCurrentFrame() tạo SDL_Rect nguồn. AssetRenderer truyền vùng nguồn này
 * vào SDL_RenderCopyEx để SDL chỉ lấy đúng frame cần vẽ.
 *
 * @param renderer SDL renderer đích.
 * @param texture Texture chứa spritesheet player.
 * @param player Player cung cấp vị trí, kích thước và hướng nhìn.
 * @param offsetX Độ lệch ngang khi render qua camera.
 * @param offsetY Độ lệch dọc khi render qua camera.
 */
void ActorRenderer::renderPlayer(SDL_Renderer* renderer,
                                 SDL_Texture* texture,
                                 const Player& player,
                                 int offsetX,
                                 int offsetY) {
    if (renderer == nullptr || texture == nullptr ||
        !player.isAlive() || player.getState() == PlayerState::Dead) {
            return;
    }
    
    SDL_Rect source = animationFor(currentState).getCurrentFrame();

    const int renderWidth = source.w * kPlayerRenderScale;
    const int renderHeight = source.h * kPlayerRenderScale;
    SDL_Rect destination {
        static_cast<int>(player.getX()) +
            (player.getWidth() - renderWidth) / 2 + offsetX,
        static_cast<int>(player.getY()) +
            player.getHeight() - renderHeight + offsetY,
        renderWidth,
        renderHeight
    };

    const SDL_RendererFlip flip =
        player.getDirection() == Direction::Left
            ? SDL_FLIP_HORIZONTAL
            : SDL_FLIP_NONE;
    
    assetRenderer.render(
        renderer, texture, &source, &destination, flip
    );
}
