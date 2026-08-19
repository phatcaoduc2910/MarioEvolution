#include "view/ActorRenderer.h"
#include "model/Boss.h"
#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/Player.h"
#include "model/World.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr int kPlayerRenderScale = 1;
constexpr int kEnemyRenderScale = 1;
constexpr int kBossRenderScale = 1;
constexpr Uint32 kDamageBlinkIntervalMs = 100;
constexpr const char* kGoombaDeathTextureId = "goomba.death";

constexpr const char* kThrowFrame1TextureId = "mario.fire.throw.1";
constexpr const char* kThrowFrame2TextureId = "mario.fire.throw.2";
constexpr const char* kTransformTextureId = "mario.super.transform";

constexpr int kTransformDurationMs = 300;
constexpr int kHurtDurationMs = 350;
constexpr int kTurnDurationMs = 400;
constexpr int kThrowFrameDurationMs = 120;
constexpr double kLandingSmokeMinSpeedPixelsPerSecond = 260.0;
constexpr double kKickSmokeTrailPixels = 10.0;

// Soi hộp va chạm
constexpr bool kDebugCollision = false;

void drawDebugBox(SDL_Renderer* renderer, const Rectangle& box,
                  int offsetX, int offsetY,
                  Uint8 red, Uint8 green, Uint8 blue) {
    const SDL_Rect outline{
        static_cast<int>(box.x) + offsetX,
        static_cast<int>(box.y) + offsetY,
        box.width,
        box.height
    };

    SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &outline);
}

// Sprite cao hơn hitbox nên neo đáy để chân nhân vật không bị nhảy.
bool anchoredDestination(SDL_Texture* texture, const GameObject& object,
                         int scale, int offsetX, int offsetY,
                         SDL_Rect& destination) {
    int frameWidth = 0;
    int frameHeight = 0;
    if (SDL_QueryTexture(
            texture, nullptr, nullptr, &frameWidth, &frameHeight) < 0) {
        return false;
    }

    const int renderWidth = frameWidth * scale;
    const int renderHeight = frameHeight * scale;
    destination = {
        static_cast<int>(object.getX()) +
            (object.getWidth() - renderWidth) / 2 + offsetX,
        static_cast<int>(object.getY()) +
            object.getHeight() - renderHeight + offsetY,
        renderWidth,
        renderHeight
    };
    return true;
}

int powerRank(PlayerState state) {
    switch (state) {
        case PlayerState::Fire:
            return 2;
        case PlayerState::Big:
            return 1;
        case PlayerState::Small:
        case PlayerState::Dead:
        default:
            return 0;
    }
}
}

ActorRenderer::ActorRenderer()
    : idleAnimation({"walk.1"}, 160),
      runAnimation({"walk.1", "walk.2", "walk.3"}, 100),
      jumpAnimation({"jump"}, 120),
      fallAnimation({"jump"}, 120),
      skidAnimation({"turn"}, 120),
      hurtAnimation({"turn"}, kHurtDurationMs),
      throwAnimation({kThrowFrame1TextureId, kThrowFrame2TextureId},
                     kThrowFrameDurationMs, false),
      transformAnimation({kTransformTextureId}, kTransformDurationMs, false),
      deathAnimation({"death"}, 240),
      goombaAnimation({"goomba.walk.1", "goomba.walk.2"}, 160),
      koopaWalkAnimation({"walk.1", "walk.2"}, 160),
      koopaShellAnimation({"shell.1", "shell.2", "shell.3", "shell.4"}, 80),
      piranhaAnimation({"piranha.plant.1", "piranha.plant.2"}, 260),
      bossIdleAnimation(makeBossAnimation(BossAction::Idle)),
      bossThrowWindUpAnimation(
          makeBossAnimation(BossAction::WindUpThrow)),
      bossThrowAnimation(makeBossAnimation(BossAction::Throw)),
      bossRecoverAnimation(makeBossAnimation(BossAction::Recover)),
      bossChargeAnimation(makeBossAnimation(BossAction::Charge)),
      bossSlamAnimation(makeBossAnimation(BossAction::GroundSlam)),
      bossHurtAnimation(makeBossAnimation(BossAction::Hurt)),
      bossEnragedAnimation(makeBossAnimation(BossAction::Enraged)),
      bossDodgeAnimation(makeBossAnimation(BossAction::Dodge)),
      bossDeathAnimation(makeBossAnimation(BossAction::Death)),
      currentState(PlayerAnimationState::Idle),
      bossAction(BossAction::Idle),
      lastPowerState(PlayerState::Small),
      wasOnGround(false),
      wasAlive(true),
      transformRemainingMs(0),
      hurtRemainingMs(0),
      turnRemainingMs(0),
      turnDirection(Direction::Right) {}

SpriteAnimation& ActorRenderer::animationFor(PlayerAnimationState state) {
    switch (state) {
        case PlayerAnimationState::Run:
            return runAnimation;
        case PlayerAnimationState::Jump:
            return jumpAnimation;
        case PlayerAnimationState::Fall:
            return fallAnimation;
        case PlayerAnimationState::Skid:
            return skidAnimation;
        case PlayerAnimationState::Hurt:
            return hurtAnimation;
        case PlayerAnimationState::Throw:
            return throwAnimation;
        case PlayerAnimationState::Transform:
            return transformAnimation;
        case PlayerAnimationState::Death:
            return deathAnimation;
        case PlayerAnimationState::Idle:
        default:
            return idleAnimation;
    }
}

SpriteAnimation& ActorRenderer::bossAnimationFor(BossAction action) {
    switch (action) {
        case BossAction::WindUpThrow:
            return bossThrowWindUpAnimation;
        case BossAction::Throw:
            return bossThrowAnimation;
        case BossAction::Recover:
            return bossRecoverAnimation;
        case BossAction::Charge:
            return bossChargeAnimation;
        case BossAction::GroundSlam:
            return bossSlamAnimation;
        case BossAction::Hurt:
            return bossHurtAnimation;
        case BossAction::Enraged:
            return bossEnragedAnimation;
        case BossAction::Dodge:
            return bossDodgeAnimation;
        case BossAction::Death:
            return bossDeathAnimation;
        case BossAction::Idle:
        default:
            return bossIdleAnimation;
    }
}

const SpriteAnimation& ActorRenderer::bossAnimationFor(
    BossAction action) const {
    return const_cast<ActorRenderer*>(this)->bossAnimationFor(action);
}

std::string ActorRenderer::variantPrefix(PlayerState state) {
    switch (state) {
        case PlayerState::Big:
            return "mario.super";
        case PlayerState::Fire:
            return "mario.fire";
        case PlayerState::Small:
        case PlayerState::Dead:
        default:
            return "mario.small";
    }
}

std::string ActorRenderer::playerFrameId() {
    const std::string& frame = animationFor(currentState).getCurrentFrameId();

    if (currentState == PlayerAnimationState::Throw ||
        currentState == PlayerAnimationState::Transform) {
        return frame;
    }

    return variantPrefix(lastPowerState) + "." + frame;
}

void ActorRenderer::detectPlayerTransitions(const Player& player, int deltaMs) {
    transformRemainingMs = std::max(0, transformRemainingMs - deltaMs);
    hurtRemainingMs = std::max(0, hurtRemainingMs - deltaMs);
    turnRemainingMs = std::max(0, turnRemainingMs - deltaMs);

    const Rectangle bounds = player.getBounds();
    const double centerX = bounds.x + bounds.width / 2.0;
    const double bottomY = bounds.y + bounds.height;
    const bool alive = player.isAlive();

    if (wasAlive && !alive) {
        effects.spawnSmoke(centerX, bottomY);
    }
    if (!wasAlive && alive) {
        transformRemainingMs = 0;
        hurtRemainingMs = 0;
        turnRemainingMs = 0;
        lastPowerState = player.getState();
        wasOnGround = player.isOnGround();
    }
    wasAlive = alive;

    if (alive && player.getState() != lastPowerState) {
        if (powerRank(player.getState()) > powerRank(lastPowerState)) {
            transformRemainingMs = kTransformDurationMs;
            transformAnimation.reset();
        } else {
            hurtRemainingMs = kHurtDurationMs;
        }
        effects.spawnSmoke(centerX, bottomY);
        lastPowerState = player.getState();
    }

    if (!wasOnGround && player.isOnGround() &&
        std::abs(player.getLandingImpactSpeed()) >
            kLandingSmokeMinSpeedPixelsPerSecond) {
        effects.spawnSmoke(centerX, bottomY);
    }
    wasOnGround = player.isOnGround();

    if (currentState != PlayerAnimationState::Skid && isPlayerSkidding(player)) {
        turnRemainingMs = kTurnDurationMs;
        turnDirection = player.getDirection();
        effects.spawnSmoke(centerX, bottomY);
    }
}

void ActorRenderer::updatePlayer(const Player& player, int deltaMs) {
    detectPlayerTransitions(player, deltaMs);

    const PlayerAnimationState nextState = selectPlayerAnimationState(
        player, transformRemainingMs > 0, hurtRemainingMs > 0,
        turnRemainingMs > 0);

    if (nextState != currentState) {
        currentState = nextState;
        animationFor(currentState).reset();
    }

    animationFor(currentState).update(deltaMs);
}

void ActorRenderer::updateBoss(BossAction action, int deltaMs) {
    if (action != bossAction) {
        bossAction = action;
        bossAnimationFor(bossAction).reset();
    }

    bossAnimationFor(bossAction).update(deltaMs);
}

bool ActorRenderer::isBossAnimationFinished() const {
    return bossAnimationFor(bossAction).isFinished();
}

void ActorRenderer::applyVisualEvent(const VisualEvent& event) {
    switch (event.type) {
        case VisualEventType::ShellKicked: {
            const double trail = event.direction == Direction::Left
                                     ? kKickSmokeTrailPixels
                                     : -kKickSmokeTrailPixels;
            effects.spawnSmoke(event.x + trail, event.y);
            break;
        }
        case VisualEventType::EnemyStomped:
        case VisualEventType::BrickBroken:
            effects.spawnSmoke(event.x, event.y);
            break;
        case VisualEventType::ShellImpact:
        case VisualEventType::FireballImpact:
        case VisualEventType::BossSlamImpact:
            effects.spawnImpact(event.x, event.y);
            break;
        case VisualEventType::BossDodge:
            effects.spawnSmoke(event.x, event.y);
            break;
    }
}

void ActorRenderer::updateWorld(World& world, int deltaMs) {
    if (const GorillaBoss* boss = world.getBoss()) {
        updateBoss(boss->getAction(), deltaMs);
    }

    goombaAnimation.update(deltaMs);
    koopaWalkAnimation.update(deltaMs);
    koopaShellAnimation.update(deltaMs);
    piranhaAnimation.update(deltaMs);

    for (const VisualEvent& event : world.getVisualEvents()) {
        applyVisualEvent(event);
    }
    world.clearVisualEvents();

    effects.update(deltaMs);
}

void ActorRenderer::renderPlayer(SDL_Renderer* renderer,
                                 const TextureManager& textures,
                                 const Player& player,
                                 int offsetX,
                                 int offsetY) {
    if (renderer == nullptr) {
        return;
    }
    if (player.isAlive() && player.isInvincible() &&
        (SDL_GetTicks() / kDamageBlinkIntervalMs) % 2 == 0) {
        return;
    }

    SDL_Texture* texture = textures.getTexture(playerFrameId());
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destination{};
    if (!anchoredDestination(texture, player, kPlayerRenderScale,
                             offsetX, offsetY, destination)) {
        return;
    }

    const Direction facing = currentState == PlayerAnimationState::Skid
                                 ? turnDirection
                                 : player.getDirection();
    const SDL_RendererFlip flip = facing == Direction::Left
                                      ? SDL_FLIP_HORIZONTAL
                                      : SDL_FLIP_NONE;

    assetRenderer.render(renderer, texture, nullptr, &destination, flip);

    if constexpr (kDebugCollision) {
        Uint8 oldRed;
        Uint8 oldGreen;
        Uint8 oldBlue;
        Uint8 oldAlpha;
        SDL_GetRenderDrawColor(
            renderer, &oldRed, &oldGreen, &oldBlue, &oldAlpha);

        const Rectangle renderBox{
            static_cast<double>(destination.x - offsetX),
            static_cast<double>(destination.y - offsetY),
            destination.w,
            destination.h
        };
        drawDebugBox(renderer, renderBox, offsetX, offsetY, 80, 80, 255);
        drawDebugBox(
            renderer, player.getBounds(), offsetX, offsetY, 0, 255, 0);
        drawDebugBox(
            renderer, player.getFeetBounds(), offsetX, offsetY, 255, 0, 0);

        SDL_SetRenderDrawColor(
            renderer, oldRed, oldGreen, oldBlue, oldAlpha);
    }
}

void ActorRenderer::renderEnemies(SDL_Renderer* renderer,
                                  const TextureManager& textures,
                                  const World& world,
                                  int offsetX,
                                  int offsetY) {
    if (renderer == nullptr) {
        return;
    }

    for (const auto& actor : world.getActors()) {
        if (const auto* fireball =
                dynamic_cast<const Fireball*>(actor.get())) {
            renderFireball(renderer, textures, *fireball, offsetX, offsetY);
        } else if (const auto* goomba =
                       dynamic_cast<const Goomba*>(actor.get())) {
            renderGoomba(renderer, textures, *goomba, offsetX, offsetY);
        } else if (const auto* koopa = dynamic_cast<const Koopa*>(actor.get())) {
            renderKoopa(renderer, textures, *koopa, offsetX, offsetY);
        } else if (const auto* plant =
                       dynamic_cast<const PiranhaPlant*>(actor.get())) {
            renderPiranha(renderer, textures, *plant, offsetX, offsetY);
        } else if (const auto* boss =
                       dynamic_cast<const GorillaBoss*>(actor.get())) {
            renderBoss(renderer, textures, *boss, offsetX, offsetY);
        }
    }
}

void ActorRenderer::renderBoss(SDL_Renderer* renderer,
                               const TextureManager& textures,
                               const GorillaBoss& boss,
                               int offsetX,
                               int offsetY) {
    if (renderer == nullptr) {
        return;
    }

    SDL_Texture* texture =
        textures.getTexture(bossAnimationFor(bossAction).getCurrentFrameId());
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destination{};
    if (!anchoredDestination(texture, boss, kBossRenderScale,
                             offsetX, offsetY, destination)) {
        return;
    }

    // Sprite sheet vẽ boss quay phải nên chỉ lật khi boss nhìn trái.
    const SDL_RendererFlip flip = boss.getDirection() == Direction::Left
                                      ? SDL_FLIP_HORIZONTAL
                                      : SDL_FLIP_NONE;
    assetRenderer.render(renderer, texture, nullptr, &destination, flip);
}

void ActorRenderer::renderEffects(SDL_Renderer* renderer,
                                  const TextureManager& textures,
                                  int offsetX,
                                  int offsetY) {
    effects.render(renderer, textures, offsetX, offsetY);
}

void ActorRenderer::renderFireball(SDL_Renderer* renderer,
                                   const TextureManager& textures,
                                   const Fireball& fireball,
                                   int offsetX,
                                   int offsetY) {
    if (!fireball.isAlive()) {
        return;
    }

    const SDL_Rect destination{
        static_cast<int>(fireball.getX()) + offsetX,
        static_cast<int>(fireball.getY()) + offsetY,
        fireball.getWidth(),
        fireball.getHeight()
    };
    assetRenderer.render(
        renderer, textures.getTexture("flame"), nullptr, &destination);
}

void ActorRenderer::renderGoomba(SDL_Renderer* renderer,
                                 const TextureManager& textures,
                                 const Goomba& goomba,
                                 int offsetX,
                                 int offsetY) {
    const std::string frameId = goomba.isAlive()
                                    ? goombaAnimation.getCurrentFrameId()
                                    : kGoombaDeathTextureId;

    SDL_Texture* texture = textures.getTexture(frameId);
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destination{};
    if (!anchoredDestination(texture, goomba, kEnemyRenderScale,
                             offsetX, offsetY, destination)) {
        return;
    }

    assetRenderer.render(renderer, texture, nullptr, &destination);
}

void ActorRenderer::renderKoopa(SDL_Renderer* renderer,
                                const TextureManager& textures,
                                const Koopa& koopa,
                                int offsetX,
                                int offsetY) {
    const std::string prefix = (koopa.getColor() == KoopaColor::Green)
                                   ? "koopa.green."
                                   : "koopa.red.";
    std::string frameId;
    SDL_RendererFlip flip = koopa.getDirection() == Direction::Left
                                ? SDL_FLIP_HORIZONTAL
                                : SDL_FLIP_NONE;

    if (!koopa.isAlive()) {
        frameId = prefix + "shell.1";
        flip = SDL_FLIP_VERTICAL;
    } else if (koopa.isSlidingShell()) {
        frameId = prefix + koopaShellAnimation.getCurrentFrameId();
    } else if (koopa.isShell()) {
        frameId = prefix + "shell.1";
    } else {
        frameId = prefix + koopaWalkAnimation.getCurrentFrameId();
    }

    SDL_Texture* texture = textures.getTexture(frameId);
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destination{};
    if (!anchoredDestination(texture, koopa, kEnemyRenderScale,
                             offsetX, offsetY, destination)) {
        return;
    }

    assetRenderer.render(renderer, texture, nullptr, &destination, flip);
}

void ActorRenderer::renderPiranha(SDL_Renderer* renderer,
                                  const TextureManager& textures,
                                  const PiranhaPlant& plant,
                                  int offsetX,
                                  int offsetY) {
    if (!plant.isAlive() || plant.getHeight() <= 0) {
        return;
    }

    SDL_Texture* texture =
        textures.getTexture(piranhaAnimation.getCurrentFrameId());
    if (texture == nullptr) {
        return;
    }

    const SDL_Rect source{
        0, 0, PiranhaPlant::kPlantWidth, plant.getHeight()};
    const SDL_Rect destination{
        static_cast<int>(plant.getX()) + offsetX,
        static_cast<int>(plant.getY()) + offsetY,
        plant.getWidth(),
        plant.getHeight()
    };

    assetRenderer.render(renderer, texture, &source, &destination);
}
