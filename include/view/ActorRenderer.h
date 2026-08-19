#pragma once

#include "AssetRenderer.h"
#include "BossAnimations.h"
#include "EffectManager.h"
#include "PlayerAnimationState.h"
#include "SpriteAnimation.h"
#include "TextureManager.h"

#include <SDL2/SDL.h>
#include <string>

class GameObject;
class GorillaBoss;
class Goomba;
class Fireball;
class Koopa;
class PiranhaPlant;
class Player;
class World;

class ActorRenderer {
public:
    ActorRenderer();

    void updatePlayer(const Player& player, int deltaMs);
    void updateWorld(World& world, int deltaMs);
    // Hook cho boss: nhận BossAction của model, view lo frame và flip.
    void updateBoss(BossAction action, int deltaMs);
    bool isBossAnimationFinished() const;

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
    void renderEffects(SDL_Renderer* renderer,
                       const TextureManager& textures,
                       int offsetX = 0,
                       int offsetY = 0);

private:
    void detectPlayerTransitions(const Player& player, int deltaMs);
    void applyVisualEvent(const VisualEvent& event);
    SpriteAnimation& animationFor(PlayerAnimationState state);
    SpriteAnimation& bossAnimationFor(BossAction action);
    const SpriteAnimation& bossAnimationFor(BossAction action) const;
    std::string playerFrameId();
    static std::string variantPrefix(PlayerState state);

    void renderGoomba(SDL_Renderer* renderer,
                      const TextureManager& textures,
                      const Goomba& goomba,
                      int offsetX,
                      int offsetY);
    void renderFireball(SDL_Renderer* renderer,
                        const TextureManager& textures,
                        const Fireball& fireball,
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
    void renderBoss(SDL_Renderer* renderer,
                    const TextureManager& textures,
                    const GorillaBoss& boss,
                    int offsetX,
                    int offsetY);

    AssetRenderer assetRenderer;
    EffectManager effects;
    SpriteAnimation idleAnimation;
    SpriteAnimation runAnimation;
    SpriteAnimation jumpAnimation;
    SpriteAnimation fallAnimation;
    SpriteAnimation skidAnimation;
    SpriteAnimation hurtAnimation;
    SpriteAnimation throwAnimation;
    SpriteAnimation transformAnimation;
    SpriteAnimation deathAnimation;
    SpriteAnimation goombaAnimation;
    SpriteAnimation koopaWalkAnimation;
    SpriteAnimation koopaShellAnimation;
    SpriteAnimation piranhaAnimation;
    SpriteAnimation bossIdleAnimation;
    SpriteAnimation bossThrowWindUpAnimation;
    SpriteAnimation bossThrowAnimation;
    SpriteAnimation bossRecoverAnimation;
    SpriteAnimation bossChargeAnimation;
    SpriteAnimation bossSlamAnimation;
    SpriteAnimation bossHurtAnimation;
    SpriteAnimation bossEnragedAnimation;
    SpriteAnimation bossDodgeAnimation;
    SpriteAnimation bossDeathAnimation;
    PlayerAnimationState currentState;
    BossAction bossAction;

    PlayerState lastPowerState;
    bool wasOnGround;
    bool wasAlive;
    int transformRemainingMs;
    int hurtRemainingMs;
    int turnRemainingMs;
    Direction turnDirection;
};
