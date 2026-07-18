#pragma once

#include "AssetRenderer.h"
#include "SpriteAnimation.h"

#include <SDL2/SDL.h>

class Player;

/**
 * Chọn animation và vẽ player từ spritesheet lên SDL renderer.
 */
class ActorRenderer {
public:
    ActorRenderer();

    void updatePlayer(const Player& player, int deltaMs);
    void renderPlayer(SDL_Renderer* renderer,
                      SDL_Texture* texture,
                      const Player& player);

private:
    enum class PlayerAnimationState {
        Idle,
        Walk,
        Jump
    };

    PlayerAnimationState selectState(const Player& player) const;
    SpriteAnimation& animationFor(PlayerAnimationState state);

    AssetRenderer assetRenderer;
    SpriteAnimation idleAnimation;
    SpriteAnimation walkAnimation;
    SpriteAnimation jumpAnimation;
    PlayerAnimationState currentState;
};
