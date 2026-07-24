#include "view/ActorRenderer.h"
#include "model/Player.h"

#include <cmath>

namespace {
constexpr SpriteSheetLayout kPlayerSpriteLayout{
    0, 16, 14, 32, 16, 32
};
constexpr int kPlayerRenderScale = 2;
}

ActorRenderer::ActorRenderer()
    : idleAnimation(kPlayerSpriteLayout, 0, 0, 8, 160),
      walkAnimation(kPlayerSpriteLayout, 2, 0, 4, 100),
      jumpAnimation(kPlayerSpriteLayout, 1, 0, 4, 120),
      currentState(PlayerAnimationState::Idle) {}

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

void ActorRenderer::updatePlayer(const Player& player, int deltaMs) {
    const PlayerAnimationState nextState = selectState(player);

    if (nextState != currentState) {
        currentState = nextState;
        animationFor(currentState).reset();
    }

    animationFor(currentState).update(deltaMs);
}

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
    // Sprite cao hơn hitbox nên neo đáy để chân Mario không bị nhảy.
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
