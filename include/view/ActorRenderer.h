#pragma once

#include "AssetRenderer.h"
#include "SpriteAnimation.h"

#include <SDL2/SDL.h>

class Player;

class ActorRenderer {
public:
    ActorRenderer();

    void updatePlayer(const Player& player, int deltaMs);
    void renderPlayer(SDL_Renderer* renderer,
                      SDL_Texture* texture,
                      const Player& player,
                      int offsetX = 0,
                      int offsetY = 0);

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
