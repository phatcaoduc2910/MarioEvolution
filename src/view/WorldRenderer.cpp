#include "view/WorldRenderer.h"

#include "model/Brick.h"
#include "model/Flag.h"
#include "model/Item.h"
#include "model/World.h"
#include "view/TileCatalog.h"
#include "view/UiRenderer.h"

#include <string>

namespace {
    constexpr const char* OPENED_BRICK_TEXTURE_ID = "gold";
    constexpr const char* FIRE_FLOWER_TEXTURE_ID = "flower";
    constexpr const char* SKY_TEXTURE_ID = "sky";
    constexpr const char* MUSHROOM_TEXTURE_ID = "ui.hud.life";
    constexpr const char* BACKGROUND_LAYER_IDS[]{"background.1", "background.2"};

    constexpr SDL_Color kHudTextColor{255, 255, 255, 255};
    constexpr int kHudMargin = 12;

    SDL_Rect destination(const GameObject& object) {
        return {
            static_cast<int>(object.getX()),
            static_cast<int>(object.getY()),
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

    SDL_Texture* sky = textures.getTexture(SKY_TEXTURE_ID);
    if (sky != nullptr) {
        const SDL_Rect dst{0, 0, viewWidth, viewHeight};
        assetRenderer.render(renderer, sky, nullptr, &dst);
    }

    int layerX = 0;
    for (const char* layerId : BACKGROUND_LAYER_IDS) {
        SDL_Texture* layer = textures.getTexture(layerId);
        if (layer == nullptr) {
            continue;
        }

        int textureWidth = 0;
        int textureHeight = 0;
        if (SDL_QueryTexture(layer, nullptr, nullptr,
                             &textureWidth, &textureHeight) < 0 ||
            textureHeight <= 0) {
            continue;
        }

        const SDL_Rect dst{
            layerX, 0, textureWidth * viewHeight / textureHeight, viewHeight};
        assetRenderer.render(renderer, layer, nullptr, &dst);
        layerX += dst.w;
    }
}

void WorldRenderer::render(SDL_Renderer* renderer, const TextureManager& textures, const World& world) {
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
            const SDL_Rect dst = flagDestination(destination(*flag));
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

        const SDL_Rect dst = destination(*brick);
        assetRenderer.render(
            renderer, textures.getTexture(textureId), nullptr, &dst);
    }

    for (const auto& item : world.getItems()) {
        if (item->isCollected()) {
            continue;
        }

        const SDL_Rect dst = destination(*item);
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

void WorldRenderer::renderHud(SDL_Renderer* renderer, const World& world) {
    if (renderer == nullptr) {
        return;
    }

    UiRenderer::drawText(
        renderer, "SCORE", kHudMargin, kHudMargin, 2, kHudTextColor);
    UiRenderer::drawText(
        renderer,
        std::to_string(world.getScore()),
        kHudMargin,
        kHudMargin + 14,
        2,
        kHudTextColor);
}
