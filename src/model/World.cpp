#include "model/World.h"

#include "model/Actor.h"
#include "model/Boss.h"
#include "model/Brick.h"
#include "model/Enemy.h"
#include "model/Fireball.h"
#include "model/Flag.h"
#include "model/LevelData.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace {
constexpr double kIntegratedFixedStepSeconds = 0.011;
constexpr double kLevelDurationSeconds = 600.0;
constexpr int kStartingLives = 3;
constexpr int kTimeBonusPerSecond = 10;
constexpr std::size_t kMaxPendingVisualEvents = 64;
constexpr int kArenaBorderTiles = 1;
constexpr double kThrowSpeedX = 220.0;
constexpr double kThrowSpeedY = -520.0;
// Koopa thả từ trên rơi ngoài khung hình một đoạn để Mario kịp thấy nó tới.
constexpr double kFallingKoopaSpawnHeight = 96.0;
}

World::World()
    : player(100.0, 550.0 - Player::kSmallHeight),
      boss(nullptr),
      arenaMinX(0.0),
      arenaMaxX(0.0),
      arenaTopY(0.0),
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
    visualEvents.clear();
    boss = nullptr;
    piranhaSpawnPoints.clear();
    arenaMinX = static_cast<double>(kArenaBorderTiles * kObjectTileSize);
    arenaMaxX = static_cast<double>(
        (level.getWidth() - kArenaBorderTiles) * kObjectTileSize);
    arenaTopY = -kFallingKoopaSpawnHeight;
    // Spawn chỉ đọc từ marker 'P' của LevelData; map thiếu marker giữ chỗ
    // đứng mặc định cũ.
    player = Player(100.0, 550.0 - Player::kSmallHeight);
    int spawnColumn = 0;
    int spawnRow = 0;
    if (level.findPlayerSpawn(spawnColumn, spawnRow)) {
        player = Player(
            static_cast<double>(spawnColumn * kObjectTileSize),
            static_cast<double>((spawnRow + 1) * kObjectTileSize) -
                Player::kSmallHeight);
    }
    remainingCoins = 0;
    timeRemainingSeconds = kLevelDurationSeconds;
    gameOver = false;
    levelComplete = false;
    killPlaneY = static_cast<double>(level.getHeight() * level.getTileSize()) + 96.0;

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
                    // Marker spawn không sinh object; player đã đặt ở trên.
                    break;
                case kBossSpawnTileId:
                    addActor(std::make_unique<GorillaBoss>(
                        x,
                        y + kObjectTileSize - GorillaBoss::kBossHeight));
                    break;
                case kPiranhaSpawnPointTileId:
                    // Chỉ ghi vị trí; arena controller mới quyết định lúc mọc.
                    piranhaSpawnPoints.push_back(
                        {x, y + kObjectTileSize});
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
    if (auto* spawnedBoss = dynamic_cast<GorillaBoss*>(actor.get())) {
        boss = spawnedBoss;
        boss->setMovementRegion(arenaMinX, arenaMaxX);
    }
    actors.push_back(std::move(actor));
}

GorillaBoss* World::getBoss() const {
    return boss;
}

Koopa& World::spawnThrownKoopa(double spawnX, double spawnY,
                               double velocityX, double velocityY) {
    auto koopa = std::make_unique<Koopa>(spawnX, spawnY, KoopaColor::Green);
    Koopa& spawned = *koopa;
    spawned.throwWith(velocityX, velocityY);
    addActor(std::move(koopa));
    return spawned;
}

Koopa& World::spawnFallingKoopa(double spawnX) {
    return spawnThrownKoopa(spawnX, arenaTopY, 0.0, 0.0);
}

PiranhaPlant* World::activatePiranhaAt(std::size_t spawnIndex) {
    if (spawnIndex >= piranhaSpawnPoints.size()) {
        return nullptr;
    }

    const PiranhaSpawnPoint& point = piranhaSpawnPoints[spawnIndex];
    auto plant = std::make_unique<PiranhaPlant>(point.x, point.mouthY);
    PiranhaPlant& spawned = *plant;
    spawned.activateOnce();
    addActor(std::move(plant));
    // Warning trước khi cây mọc: hiệu ứng khói ngay tại miệng hố.
    emitVisualEvent(VisualEventType::EnemyStomped,
                    point.x + PiranhaPlant::kPlantWidth / 2.0, point.mouthY);
    return &spawned;
}

const std::vector<PiranhaSpawnPoint>& World::getPiranhaSpawnPoints() const {
    return piranhaSpawnPoints;
}

int World::countActiveKoopas() const {
    int count = 0;
    for (const auto& actor : actors) {
        const auto* koopa = dynamic_cast<const Koopa*>(actor.get());
        if (koopa != nullptr && koopa->isAlive()) {
            ++count;
        }
    }
    return count;
}

int World::countActivePiranhas() const {
    int count = 0;
    for (const auto& actor : actors) {
        const auto* plant = dynamic_cast<const PiranhaPlant*>(actor.get());
        if (plant != nullptr && plant->isAlive()) {
            ++count;
        }
    }
    return count;
}

// Dọn hazard khi boss chết: Koopa và Piranha không được sống tiếp qua victory.
void World::clearHazards() {
    for (auto& actor : actors) {
        if (actor.get() == static_cast<Actor*>(boss)) {
            continue;
        }
        auto* enemy = dynamic_cast<Enemy*>(actor.get());
        if (enemy != nullptr && enemy->isAlive()) {
            enemy->die();
        }
    }
}

double World::getArenaMinX() const {
    return arenaMinX;
}

double World::getArenaMaxX() const {
    return arenaMaxX;
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

void World::emitVisualEvent(VisualEventType type, double x, double y,
                            Direction direction) {
    if (visualEvents.size() >= kMaxPendingVisualEvents) {
        return;
    }

    visualEvents.push_back({type, x, y, direction});
}

const std::vector<VisualEvent>& World::getVisualEvents() const {
    return visualEvents;
}

void World::clearVisualEvents() {
    visualEvents.clear();
}

// Chuyển fireball vào actor để thêm di chuyển và tương tác
bool World::shootFireball() {
    std::unique_ptr<Fireball> fireball = player.shootFireball();
    if (fireball == nullptr) {
        return false;
    }

    addActor(std::move(fireball));
    return true;
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

// Boss chỉ phát yêu cầu; việc tạo Koopa và effect là của World.
void World::updateBoss() {
    if (boss == nullptr) {
        return;
    }

    const Rectangle bounds = boss->getBounds();
    const double centerX = bounds.x + bounds.width / 2.0;

    if (boss->consumeThrowRequest()) {
        const bool toLeft = boss->getThrowDirection() == Direction::Left;
        const double handX = toLeft
                                 ? bounds.x - Koopa::kWalkWidth
                                 : bounds.x + bounds.width;
        spawnThrownKoopa(
            handX,
            bounds.y + bounds.height / 3.0,
            toLeft ? -kThrowSpeedX : kThrowSpeedX,
            kThrowSpeedY);
    }

    if (boss->consumeSlamImpactCue()) {
        emitVisualEvent(VisualEventType::BossSlamImpact, centerX,
                        bounds.y + bounds.height);
    }

    if (boss->consumeDodgeCue()) {
        emitVisualEvent(VisualEventType::BossDodge, centerX,
                        bounds.y + bounds.height);
    }

    if (boss->consumeDeathCue()) {
        emitVisualEvent(VisualEventType::BossSlamImpact, centerX,
                        bounds.y + bounds.height / 2.0);
        emitVisualEvent(VisualEventType::BossDodge, centerX,
                        bounds.y + bounds.height);
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
    // Xét respawnX dựa trên vị trí Mario chạm đất gần nhất
    const double respawnX =
        player.isAlive() && player.isOnGround() &&
                player.getY() <= killPlaneY
            ? player.getX()
            : safePlayerX;
    update(dtSeconds, respawnX);
}

void World::update(double dtSeconds, double respawnX) {
    safePlayerX = std::max(0.0, respawnX);
    if (player.isAlive() && player.isOnGround() &&
        player.getY() <= killPlaneY) {
        safePlayerY = player.getY();
    }

    if (!gameOver && !levelComplete && dtSeconds > 0.0 && player.isAlive()) {
        timeRemainingSeconds = std::max(
            0.0, timeRemainingSeconds - dtSeconds);
        if (timeRemainingSeconds <= 0.0) {
            loseLife(safePlayerX, safePlayerY);
            if (!gameOver) {
                timeRemainingSeconds = kLevelDurationSeconds;
            }
            return;
        }
    }

    player.update(dtSeconds);
    if (!player.isAlive()) {
        player.moveY(dtSeconds);
    }

    if (boss != nullptr) {
        boss->trackPlayerCenter(player.getX() + player.getWidth() / 2.0);
    }

    for (auto& actor : actors) {
        actor->update(dtSeconds);
        if (actor->isAlive()) {
            // Huỷ fireball khi rơi xuống dưới kill plane  
            auto* fireball = dynamic_cast<Fireball*>(actor.get());
            if (fireball != nullptr && fireball->getY() > killPlaneY) {
                fireball->destroy();
            }
        }
    }

    for (auto& object : objects) {
        object->update(dtSeconds);
    }

    for (auto& item : items) {
        item->update(dtSeconds);
    }

    updateBoss();

    if (!player.isAlive()) {
        if (player.isDeathAnimationFinished()) {
            loseLife(safePlayerX, safePlayerY);
        }
    } else if (player.getY() > killPlaneY) {
        loseLife(safePlayerX, safePlayerY);
    }

    actors.erase(
        std::remove_if(
            actors.begin(),
            actors.end(),
            [](const std::unique_ptr<Actor>& actor) {
                return actor->isRemovable();
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
