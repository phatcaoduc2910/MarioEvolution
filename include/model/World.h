#pragma once

#include "Item.h"
#include "Player.h"
#include "StaticObject.h"

#include <memory>
#include <vector>

class Actor;
class LevelData;

class World {
public:
    World();

    Player& getPlayer();
    const std::vector<std::unique_ptr<Actor>>& getActors() const;
    const std::vector<std::unique_ptr<StaticObject>>& getObjects() const;
    const std::vector<std::unique_ptr<Item>>& getItems() const;

    void loadLevel(const LevelData& level);

    void addActor(std::unique_ptr<Actor> actor);
    void addObject(std::unique_ptr<StaticObject> object);
    void addItem(std::unique_ptr<Item> item);
    void addScore(int points);
    void collectCoin(int points);

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

    Player player;
    std::vector<std::unique_ptr<Actor>> actors;
    std::vector<std::unique_ptr<StaticObject>> objects;
    std::vector<std::unique_ptr<Item>> items;
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
