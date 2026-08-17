#include "view/ActorRenderer.h"
#include "model/Enemy.h"
#include "model/Player.h"
#include "model/World.h"

#include <cmath>

namespace {
constexpr int kPlayerRenderScale = 1;
constexpr int kEnemyRenderScale = 1;
constexpr const char* kGoombaDeathTextureId = "goomba.death";

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
}

ActorRenderer::ActorRenderer()
    : idleAnimation({"walk.1"}, 160),
      walkAnimation({"walk.1", "walk.2", "walk.3"}, 100),
      jumpAnimation({"jump"}, 120),
      goombaAnimation({"goomba.walk.1", "goomba.walk.2"}, 160),
      koopaWalkAnimation({"walk.1", "walk.2"}, 160),
      koopaShellAnimation({"shell.1", "shell.2", "shell.3", "shell.4"}, 80),
      piranhaAnimation({"piranha.plant.1", "piranha.plant.2"}, 260),
      currentState(PlayerAnimationState::Idle) {}

ActorRenderer::PlayerAnimationState
ActorRenderer::selectState(const Player& player) const {
    constexpr double kMotionEpsilon = 0.002;

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
    koopaWalkAnimation.update(deltaMs);
    koopaShellAnimation.update(deltaMs);
    piranhaAnimation.update(deltaMs);
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
        if (const auto* goomba = dynamic_cast<const Goomba*>(actor.get())) {
            renderGoomba(renderer, textures, *goomba, offsetX, offsetY);
        } else if (const auto* koopa = dynamic_cast<const Koopa*>(actor.get())) {
            renderKoopa(renderer, textures, *koopa, offsetX, offsetY);
        } else if (const auto* plant =
                       dynamic_cast<const PiranhaPlant*>(actor.get())) {
            renderPiranha(renderer, textures, *plant, offsetX, offsetY);
        }
    }
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
    if (!koopa.isAlive()) {
        return;
    }

    const std::string prefix = (koopa.getColor() == KoopaColor::Green)
                                   ? "koopa.green."
                                   : "koopa.red.";
    std::string frameId;
    if (koopa.isSlidingShell()) {
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

    const SDL_RendererFlip flip = koopa.getDirection() == Direction::Left
                                      ? SDL_FLIP_HORIZONTAL
                                      : SDL_FLIP_NONE;

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
