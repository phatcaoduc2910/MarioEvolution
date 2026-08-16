#pragma once

#include "AssetRenderer.h"
#include "SpriteAnimation.h"
#include "TextureManager.h"

#include <SDL2/SDL.h>
#include <string>

class Player;
class World;

class ActorRenderer {
public:
    ActorRenderer();

    void updatePlayer(const Player& player, int deltaMs);
    void updateEnemies(int deltaMs);

    void renderPlayer(SDL_Renderer* renderer,
                      const TextureManager& textures,
                      const Player& player,
                      int offsetX = 0,
                      int offsetY = 0);
    void renderEnemies(SDL_Renderer* renderer,
                       const TextureManager& textures,
                       const World& world,
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
    static std::string variantPrefix(const Player& player);

    AssetRenderer assetRenderer;
    SpriteAnimation idleAnimation;
    SpriteAnimation walkAnimation;
    SpriteAnimation jumpAnimation;
    SpriteAnimation goombaAnimation;
    PlayerAnimationState currentState;
};
