#include "model/World.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/LevelData.h"

#include <stdexcept>
#include <utility>

World::World()
    : player(100.0, 502.0),
      score(0),
      gameOver(false) {}


Player& World::getPlayer() {
    return player;
}

const std::vector<std::unique_ptr<Actor>>& World::getActors() const {
    return actors;
}

const std::vector<std::unique_ptr<StaticObject>>& World::getObjects() const {
    return objects;
}

const std::vector<std::unique_ptr<Item>>& World::getItems() const {
    return items;
}

/**
 * Chuyển dữ liệu map tĩnh thành các brick và item runtime thuộc sở hữu World.
 *
 * @param level Lưới map dùng tile 32 pixel.
 * @throws std::invalid_argument Khi kích thước tile không khớp object hiện tại.
 */
void World::loadLevel(const LevelData& level) {
    constexpr int kObjectTileSize = 32;
    if (level.getTileSize() != kObjectTileSize) {
        throw std::invalid_argument("World requires 32-pixel map tiles");
    }

    actors.clear();
    objects.clear();
    items.clear();
    score = 0;
    gameOver = false;

    for (int row = 0; row < level.getHeight(); ++row) {
        for (int column = 0; column < level.getWidth(); ++column) {
            const TileId tileId = level.getTile(column, row);
            const double x = static_cast<double>(column * kObjectTileSize);
            const double y = static_cast<double>(row * kObjectTileSize);

            switch (tileId) {
                case kEmptyTileId:
                    break;
                case kStandardBrickTileId:
                    addObject(std::make_unique<StandardBrick>(x, y));
                    break;
                case kCoinBrickTileId:
                    addObject(std::make_unique<CoinBrick>(x, y, 1));
                    break;
                case kMushroomBrickTileId:
                    addObject(std::make_unique<MushroomBrick>(x, y));
                    break;
                case kFlowerBrickTileId:
                    addObject(std::make_unique<FlowerBrick>(x, y));
                    break;
                case kCoinTileId:
                    addItem(std::make_unique<Coin>(x + 8.0, y + 8.0, 1));
                    break;
                default:
                    throw std::invalid_argument("World received an unknown tile id");
            }
        }
    }
}

void World::addActor(std::unique_ptr<Actor> actor) {
    actors.push_back(std::move(actor));
}

void World::addObject(std::unique_ptr<StaticObject> object) {
    objects.push_back(std::move(object));
}

void World::addItem(std::unique_ptr<Item> item) {
    items.push_back(std::move(item));
}

void World::addScore(int points) {
    score += points;
}

int World::getScore() const {
    return score;
}

bool World::isGameOver() const {
    return gameOver;
}

void World::update() {
    player.update();

    for (auto& actor : actors) {
        if (actor->isAlive()) {
            actor->update();
        }
    }

    for (auto& item : items) {
        item->update();
    }

    // Player rơi khỏi màn hình.
    if (player.getY() > 600.0) {
        gameOver = true;
    }
}
