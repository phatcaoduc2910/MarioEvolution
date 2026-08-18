#include "model/World.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Flag.h"
#include "model/LevelData.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace {
constexpr double kIntegratedFixedStepSeconds = 0.011;
constexpr double kLevelDurationSeconds = 600.0;
constexpr int kStartingLives = 3;
constexpr int kTimeBonusPerSecond = 10;
}

World::World()
    : player(100.0, 550.0 - Player::kSmallHeight),
      score(0),
      remainingCoins(0),
      lives(kStartingLives),
      timeRemainingSeconds(kLevelDurationSeconds),
      safePlayerX(player.getX()),
      safePlayerY(player.getY()),
      gameOver(false),
      levelComplete(false),
      killPlaneY(700.0) {}


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


//Đọc map từ level.map
void World::loadLevel(const LevelData& level) {
    constexpr int kObjectTileSize = 32;
    if (level.getTileSize() != kObjectTileSize) {
        throw std::invalid_argument("World requires 32-pixel map tiles");
    }

    actors.clear();
    objects.clear();
    items.clear();
    player = Player(100.0, 550.0 - Player::kSmallHeight);
    remainingCoins = 0;
    timeRemainingSeconds = kLevelDurationSeconds;
    gameOver = false;
    levelComplete = false;
    killPlaneY = static_cast<double>(level.getHeight() * level.getTileSize()) + 96.0;

    bool spawnPlaced = false;
    for (int row = 0; row < level.getHeight(); ++row) {
        for (int column = 0; column < level.getWidth(); ++column) {
            const TileId tileId = level.getTile(column, row);
            const double x = static_cast<double>(column * kObjectTileSize);
            const double y = static_cast<double>(row * kObjectTileSize);

            // Add block theo tileId
            switch (tileId) {
                case kEmptyTileId:
                    break;
                case kStandardBrickTileId:
                    addObject(std::make_unique<StandardBrick>(x, y));
                    break;
                case kCoinBrickTileId:
                    addObject(std::make_unique<CoinBrick>(x, y, 1));
                    ++remainingCoins;
                    break;
                case kMushroomBrickTileId:
                    addObject(std::make_unique<MushroomBrick>(x, y));
                    break;
                case kFlowerBrickTileId:
                    addObject(std::make_unique<FlowerBrick>(x, y));
                    break;
                case kCoinTileId:
                    addItem(std::make_unique<Coin>(
                        x + 8.0, y + 8.0, Coin::kScoreValue));
                    ++remainingCoins;
                    break;
                case kGoombaTileId:
                    addActor(std::make_unique<Goomba>(x, y));
                    break;
                case kKoopaGreenTileId:
                    addActor(std::make_unique<Koopa>(
                        x,
                        y + kObjectTileSize - Koopa::kWalkHeight,
                        KoopaColor::Green));
                    break;
                case kKoopaRedTileId:
                    addActor(std::make_unique<Koopa>(
                        x,
                        y + kObjectTileSize - Koopa::kWalkHeight,
                        KoopaColor::Red));
                    break;
                case kPiranhaTileId:
                    addActor(std::make_unique<PiranhaPlant>(
                        x, y + kObjectTileSize));
                    break;
                case kPlayerSpawnTileId:
                    if (!spawnPlaced) {
                        player = Player(
                            x, y + kObjectTileSize - Player::kSmallHeight);
                        spawnPlaced = true;
                    }
                    break;
                case kFlagTileId: {
                    const double poleX =
                        x + (kObjectTileSize - Flag::kPoleWidth) / 2.0;
                    addObject(std::make_unique<Flag>(poleX, y - 128.0));
                    break;
                }
                default:
                    throw std::invalid_argument("World received an unknown tile id");
            }
        }
    }
    safePlayerX = player.getX();
    safePlayerY = player.getY();
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

void World::collectCoin(int points) {
    addScore(points);
    remainingCoins = std::max(0, remainingCoins - 1);
}


int World::getScore() const {
    return score;
}

int World::getRemainingCoins() const {
    return remainingCoins;
}

int World::getTimeRemaining() const {
    return static_cast<int>(std::ceil(std::max(0.0, timeRemainingSeconds)));
}

int World::getLives() const {
    return lives;
}

bool World::isGameOver() const {
    return gameOver;
}

bool World::isLevelComplete() const {
    return levelComplete;
}

void World::markLevelComplete() {
    if (!gameOver && !levelComplete) {
        score += getTimeRemaining() * kTimeBonusPerSecond;
        levelComplete = true;
    }
}

void World::loseLife(double reviveX, double reviveY) {
    if (gameOver || levelComplete) {
        return;
    }
    lives = std::max(0, lives - 1);
    gameOver = lives == 0;
    if (!gameOver) {
        player.reviveAt(reviveX, reviveY);
    }
}

void World::update() {
    update(kIntegratedFixedStepSeconds);
}

void World::update(double dtSeconds) {
    if (!gameOver && !levelComplete && dtSeconds > 0.0) {
        timeRemainingSeconds = std::max(
            0.0, timeRemainingSeconds - dtSeconds);
        if (timeRemainingSeconds <= 0.0) {
            loseLife(player.getX(), player.getY());
            if (!gameOver) {
                timeRemainingSeconds = kLevelDurationSeconds;
            }
            return;
        }
    }

    if (player.isAlive() && player.isOnGround() &&
        player.getY() <= killPlaneY) {
        safePlayerX = player.getX();
        safePlayerY = player.getY();
    }

    player.update(dtSeconds);

    for (auto& actor : actors) {
        if (actor->isAlive()) {
            actor->update(dtSeconds);
        }
    }

    for (auto& object : objects) {
        object->update(dtSeconds);
    }

    for (auto& item : items) {
        item->update(dtSeconds);
    }

    if (player.getY() > killPlaneY) {
        loseLife(safePlayerX, safePlayerY);
    } else if (!player.isAlive() ||
               player.getState() == PlayerState::Dead) {
        loseLife(player.getX(), player.getY());
    }

    actors.erase(
        std::remove_if(
            actors.begin(),
            actors.end(),
            [](const std::unique_ptr<Actor>& actor) {
                return !actor->isAlive();
            }),
        actors.end());

    items.erase(
        std::remove_if(
            items.begin(),
            items.end(),
            [](const std::unique_ptr<Item>& item) {
                return item->isCollected();
            }),
        items.end());
}
