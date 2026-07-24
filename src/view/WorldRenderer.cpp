#include "view/WorldRenderer.h"

#include "model/Brick.h"
#include "model/Item.h"
#include "model/World.h"
#include "view/TileCatalog.h"

namespace {
    constexpr SDL_Rect OPENED_BRICK_SPRITE{64, 1104, 16, 16};

    SDL_Rect destination(const GameObject& object) {
        return {
            static_cast<int>(object.getX()),
            static_cast<int>(object.getY()),
            object.getWidth(),
            object.getHeight()
        };
    }

    void fill(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    void renderMushroom(SDL_Renderer* renderer, const SDL_Rect& dst) {
        const SDL_Rect cap{dst.x, dst.y, dst.w, dst.h / 2};
        const SDL_Rect stem{dst.x + dst.w / 4, dst.y + dst.h / 2, dst.w / 2, dst.h / 2};
        const SDL_Rect leftSpot{dst.x + dst.w / 6, dst.y + dst.h / 6, dst.w / 6, dst.h / 6};
        const SDL_Rect rightSpot{dst.x + dst.w * 2 / 3, dst.y + dst.h / 6, dst.w / 6, dst.h / 6};

        fill(renderer, cap, {220, 45, 45, 255});
        fill(renderer, stem, {255, 222, 173, 255});
        fill(renderer, leftSpot, {255, 255, 255, 255});
        fill(renderer, rightSpot, {255, 255, 255, 255});
    }

    void renderFireFlower(SDL_Renderer* renderer, const SDL_Rect& dst) {
        const SDL_Rect stem{dst.x + dst.w * 3 / 8, dst.y + dst.h / 2, dst.w / 4, dst.h / 2};
        const SDL_Rect petals{dst.x + dst.w / 8, dst.y, dst.w * 3 / 4, dst.h * 3 / 5};
        const SDL_Rect center{dst.x + dst.w * 3 / 8, dst.y + dst.h / 5, dst.w / 4, dst.h / 4};

        fill(renderer, stem, {40, 180, 70, 255});
        fill(renderer, petals, {235, 60, 45, 255});
        fill(renderer, center, {255, 220, 40, 255});
    }
}

void WorldRenderer::render(SDL_Renderer* renderer, SDL_Texture* worldTiles, const World& world) {
    if (renderer == nullptr || worldTiles == nullptr) {
        return;
    }

    Uint8 oldRed;
    Uint8 oldGreen;
    Uint8 oldBlue;
    Uint8 oldAlpha;
    SDL_GetRenderDrawColor(renderer, &oldRed, &oldGreen, &oldBlue, &oldAlpha);

    for (const auto& object : world.getObjects()) {
        const auto* brick = dynamic_cast<const Brick*>(object.get());
        if (brick == nullptr || (brick->canBeBroken() && brick->isOpened())) {
            continue;
        }

        const SDL_Rect* sprite = &OPENED_BRICK_SPRITE;
        if (!brick->isOpened()) {
            const TileId tileId = brick->canBeBroken()
                                      ? kStandardBrickTileId
                                      : kCoinBrickTileId;
            const TileDefinition* definition = findTileDefinition(tileId);
            if (definition == nullptr) {
                continue;
            }
            sprite = &definition->source;
        }

        const SDL_Rect dst = destination(*brick);
        assetRenderer.render(renderer, worldTiles, sprite, &dst);
    }

    for (const auto& item : world.getItems()) {
        if (item->isCollected()) {
            continue;
        }

        const SDL_Rect dst = destination(*item);
        if (dynamic_cast<const Coin*>(item.get()) != nullptr) {
            const TileDefinition* definition =
                findTileDefinition(kCoinTileId);
            if (definition != nullptr) {
                assetRenderer.render(
                    renderer, worldTiles, &definition->source, &dst);
            }
        } else if (dynamic_cast<const Mushroom*>(item.get()) != nullptr) {
            renderMushroom(renderer, dst);
        } else if (dynamic_cast<const FireFlower*>(item.get()) != nullptr) {
            renderFireFlower(renderer, dst);
        }
    }

    SDL_SetRenderDrawColor(renderer, oldRed, oldGreen, oldBlue, oldAlpha);
}
