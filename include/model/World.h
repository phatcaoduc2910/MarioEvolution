#pragma once

#include "Item.h"
#include "Player.h"
#include "StaticObject.h"

#include <cstddef>
#include <memory>
#include <vector>

class Actor;
class GorillaBoss;
class Koopa;
class LevelData;
class PiranhaPlant;

// Điểm mọc Piranha đọc từ map: x của ô và cao độ miệng cây.
struct PiranhaSpawnPoint {
    double x;
    double mouthY;
};

class World {
public:
    World();

    Player& getPlayer();
    const std::vector<std::unique_ptr<Actor>>& getActors() const;
    const std::vector<std::unique_ptr<StaticObject>>& getObjects() const;
    const std::vector<std::unique_ptr<Item>>& getItems() const;

    void loadLevel(const LevelData& level);

    // Boss arena: boss do map tạo, các API spawn dưới đây dành cho arena
    // controller nên nó không phải tự dựng behavior enemy.
    GorillaBoss* getBoss() const;
    Koopa& spawnThrownKoopa(double spawnX, double spawnY,
                            double velocityX, double velocityY);
    Koopa& spawnFallingKoopa(double spawnX);
    PiranhaPlant* activatePiranhaAt(std::size_t spawnIndex);
    const std::vector<PiranhaSpawnPoint>& getPiranhaSpawnPoints() const;
    int countActiveKoopas() const;
    int countActivePiranhas() const;
    void clearHazards();
    double getArenaMinX() const;
    double getArenaMaxX() const;

    void addActor(std::unique_ptr<Actor> actor);
    void addObject(std::unique_ptr<StaticObject> object);
    void addItem(std::unique_ptr<Item> item);
    void addScore(int points);
    void collectCoin(int points);
    bool shootFireball();

    void emitVisualEvent(VisualEventType type, double x, double y,
                         Direction direction = Direction::Right);
    const std::vector<VisualEvent>& getVisualEvents() const;
    void clearVisualEvents();

    int getScore() const;
    int getRemainingCoins() const;
    int getTimeRemaining() const;
    int getLives() const;
    bool isGameOver() const;
    bool isLevelComplete() const;
    void markLevelComplete();

    void update();
    void update(double dtSeconds);
    void update(double dtSeconds, double respawnX);

private:
    void loseLife(double reviveX, double reviveY);
    void updateBoss();

    Player player;
    GorillaBoss* boss;
    std::vector<PiranhaSpawnPoint> piranhaSpawnPoints;
    double arenaMinX;
    double arenaMaxX;
    double arenaTopY;
    std::vector<std::unique_ptr<Actor>> actors;
    std::vector<std::unique_ptr<StaticObject>> objects;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<VisualEvent> visualEvents;
    int score;
    int remainingCoins;
    int lives;
    double timeRemainingSeconds;
    double safePlayerX;
    double safePlayerY;
    bool gameOver;
    bool levelComplete;
    double killPlaneY;
};
