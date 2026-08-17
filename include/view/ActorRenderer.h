#pragma once

#include "AssetRenderer.h"
#include "SpriteAnimation.h"
#include "TextureManager.h"

#include <SDL2/SDL.h>
#include <string>

class Goomba;
class Koopa;
class PiranhaPlant;
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

    void renderGoomba(SDL_Renderer* renderer,
                      const TextureManager& textures,
                      const Goomba& goomba,
                      int offsetX,
                      int offsetY);
    void renderKoopa(SDL_Renderer* renderer,
                     const TextureManager& textures,
                     const Koopa& koopa,
                     int offsetX,
                     int offsetY);
    void renderPiranha(SDL_Renderer* renderer,
                       const TextureManager& textures,
                       const PiranhaPlant& plant,
                       int offsetX,
                       int offsetY);

    AssetRenderer assetRenderer;
    SpriteAnimation idleAnimation;
    SpriteAnimation walkAnimation;
    SpriteAnimation jumpAnimation;
    SpriteAnimation goombaAnimation;
    SpriteAnimation koopaWalkAnimation;
    SpriteAnimation koopaShellAnimation;
    SpriteAnimation piranhaAnimation;
    PlayerAnimationState currentState;
};
