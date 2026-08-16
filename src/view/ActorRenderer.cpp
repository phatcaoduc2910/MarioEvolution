#include "view/ActorRenderer.h"
#include "model/Enemy.h"
#include "model/Player.h"
#include "model/World.h"

#include <cmath>

namespace {
constexpr int kPlayerRenderScale = 1;
constexpr int kEnemyRenderScale = 1;
constexpr const char* kGoombaDeathTextureId = "goomba.death";

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
}

ActorRenderer::ActorRenderer()
    : idleAnimation({"walk.1"}, 160),
      walkAnimation({"walk.1", "walk.2", "walk.3"}, 100),
      jumpAnimation({"jump"}, 120),
      goombaAnimation({"goomba.walk.1", "goomba.walk.2"}, 200),
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

std::string ActorRenderer::variantPrefix(const Player& player) {
    switch (player.getState()) {
        case PlayerState::Big:
            return "mario.super";
        case PlayerState::Fire:
            return "mario.fire";
        case PlayerState::Small:
        default:
            return "mario.small";
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

void ActorRenderer::updateEnemies(int deltaMs) {
    goombaAnimation.update(deltaMs);
}

void ActorRenderer::renderPlayer(SDL_Renderer* renderer,
                                 const TextureManager& textures,
                                 const Player& player,
                                 int offsetX,
                                 int offsetY) {
    if (renderer == nullptr || !player.isAlive() ||
        player.getState() == PlayerState::Dead) {
        return;
    }

    const std::string frameId =
        variantPrefix(player) + '.' +
        animationFor(currentState).getCurrentFrameId();

    SDL_Texture* texture = textures.getTexture(frameId);
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destination{};
    if (!anchoredDestination(texture, player, kPlayerRenderScale,
                             offsetX, offsetY, destination)) {
        return;
    }

    const SDL_RendererFlip flip =
        player.getDirection() == Direction::Left
            ? SDL_FLIP_HORIZONTAL
            : SDL_FLIP_NONE;

    assetRenderer.render(renderer, texture, nullptr, &destination, flip);
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
        const auto* goomba = dynamic_cast<const Goomba*>(actor.get());
        if (goomba == nullptr) {
            continue;
        }

        const std::string frameId = goomba->isAlive()
                                        ? goombaAnimation.getCurrentFrameId()
                                        : kGoombaDeathTextureId;

        SDL_Texture* texture = textures.getTexture(frameId);
        if (texture == nullptr) {
            continue;
        }

        SDL_Rect destination{};
        if (!anchoredDestination(texture, *goomba, kEnemyRenderScale,
                                 offsetX, offsetY, destination)) {
            continue;
        }

        assetRenderer.render(renderer, texture, nullptr, &destination);
    }
}
