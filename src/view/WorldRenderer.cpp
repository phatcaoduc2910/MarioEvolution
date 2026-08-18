#include "view/WorldRenderer.h"

#include "model/Brick.h"
#include "model/Flag.h"
#include "model/Item.h"
#include "model/World.h"
#include "view/TileCatalog.h"

namespace {
    constexpr const char* OPENED_BRICK_TEXTURE_ID = "gold";
    constexpr const char* FIRE_FLOWER_TEXTURE_ID = "flower";
    constexpr const char* BACKGROUND_TEXTURE_ID = "background.game";
    constexpr const char* MUSHROOM_TEXTURE_ID = "ui.hud.life";

    SDL_Rect destination(const GameObject& object, int offsetX, int offsetY) {
        return {
            static_cast<int>(object.getX()) + offsetX,
            static_cast<int>(object.getY()) + offsetY,
            object.getWidth(),
            object.getHeight()
        };
    }

    constexpr int FLAG_CANVAS_SIZE = 554;
    constexpr int FLAG_POLE_LEFT = 234;
    constexpr int FLAG_POLE_TOP = 126;
    constexpr int FLAG_POLE_HEIGHT = 304;

    SDL_Rect flagDestination(const SDL_Rect& poleBox) {
        const double scale =
            static_cast<double>(poleBox.h) / FLAG_POLE_HEIGHT;
        const int canvasSize = static_cast<int>(FLAG_CANVAS_SIZE * scale);

        return {
            poleBox.x - static_cast<int>(FLAG_POLE_LEFT * scale),
            poleBox.y - static_cast<int>(FLAG_POLE_TOP * scale),
            canvasSize,
            canvasSize
        };
    }
}

void WorldRenderer::update(int deltaMs) {
    coinAnimation.update(deltaMs);
    flagAnimation.update(deltaMs);
}

void WorldRenderer::renderBackground(SDL_Renderer* renderer,
                                     const TextureManager& textures,
                                     int viewWidth, int viewHeight) {
    if (renderer == nullptr || viewWidth <= 0 || viewHeight <= 0) {
        return;
    }

    SDL_Texture* background = textures.getTexture(BACKGROUND_TEXTURE_ID);
    int textureWidth = 0;
    int textureHeight = 0;
    if (background == nullptr ||
        SDL_QueryTexture(background, nullptr, nullptr,
                         &textureWidth, &textureHeight) < 0 ||
        textureWidth <= 0 || textureHeight <= 0) {
        return;
    }

    SDL_Rect source{0, 0, textureWidth, textureHeight};
    if (textureWidth * viewHeight > textureHeight * viewWidth) {
        source.w = textureHeight * viewWidth / viewHeight;
        source.x = (textureWidth - source.w) / 2;
    } else {
        source.h = textureWidth * viewHeight / viewWidth;
        source.y = (textureHeight - source.h) / 2;
    }

    const SDL_Rect destination{0, 0, viewWidth, viewHeight};
    assetRenderer.render(renderer, background, &source, &destination);
}

void WorldRenderer::render(SDL_Renderer* renderer,
                           const TextureManager& textures,
                           const World& world,
                           int offsetX,
                           int offsetY) {
    if (renderer == nullptr) {
        return;
    }

    Uint8 oldRed;
    Uint8 oldGreen;
    Uint8 oldBlue;
    Uint8 oldAlpha;
    SDL_GetRenderDrawColor(renderer, &oldRed, &oldGreen, &oldBlue, &oldAlpha);

    for (const auto& object : world.getObjects()) {
        const auto* flag = dynamic_cast<const Flag*>(object.get());
        if (flag != nullptr) {
            const SDL_Rect dst =
                flagDestination(destination(*flag, offsetX, offsetY));
            assetRenderer.render(
                renderer,
                textures.getTexture(flagAnimation.getCurrentFrameId()),
                nullptr,
                &dst);
            continue;
        }

        const auto* brick = dynamic_cast<const Brick*>(object.get());
        if (brick == nullptr || (brick->canBeBroken() && brick->isOpened())) {
            continue;
        }

        const char* textureId = OPENED_BRICK_TEXTURE_ID;
        if (!brick->isOpened()) {
            const TileId tileId = brick->canBeBroken()
                                      ? kStandardBrickTileId
                                      : kCoinBrickTileId;
            const TileDefinition* definition = findTileDefinition(tileId);
            if (definition == nullptr || definition->textureId == nullptr) {
                continue;
            }
            textureId = definition->textureId;
        }

        const SDL_Rect dst = destination(*brick, offsetX, offsetY);
        assetRenderer.render(
            renderer, textures.getTexture(textureId), nullptr, &dst);
    }

    for (const auto& item : world.getItems()) {
        if (item->isCollected()) {
            continue;
        }

        const SDL_Rect dst = destination(*item, offsetX, offsetY);
        if (dynamic_cast<const Coin*>(item.get()) != nullptr) {
            assetRenderer.render(
                renderer,
                textures.getTexture(coinAnimation.getCurrentFrameId()),
                nullptr,
                &dst);
        } else if (dynamic_cast<const FireFlower*>(item.get()) != nullptr) {
            assetRenderer.render(
                renderer,
                textures.getTexture(FIRE_FLOWER_TEXTURE_ID),
                nullptr,
                &dst);
        } else if (dynamic_cast<const Mushroom*>(item.get()) != nullptr) {
            assetRenderer.render(
                renderer,
                textures.getTexture(MUSHROOM_TEXTURE_ID),
                nullptr,
                &dst);
        }
    }

    SDL_SetRenderDrawColor(renderer, oldRed, oldGreen, oldBlue, oldAlpha);
}
