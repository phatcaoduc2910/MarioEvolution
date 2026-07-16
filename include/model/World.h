#pragma once

#include "Item.h"
#include "Player.h"
#include "StaticObject.h"

#include <memory>
#include <vector>

class Actor;
struct SDL_Renderer;

class World {
public:
    World();

    Player& getPlayer();
    const std::vector<std::unique_ptr<Actor>>& getActors() const;
    const std::vector<std::unique_ptr<StaticObject>>& getObjects() const;
    const std::vector<std::unique_ptr<Item>>& getItems() const;

    void addActor(std::unique_ptr<Actor> actor);
    void addObject(std::unique_ptr<StaticObject> object);
    void addItem(std::unique_ptr<Item> item);
    void addScore(int points);

    int getScore() const;
    bool isGameOver() const;

    void update();
    void render(SDL_Renderer* renderer) const;

private:
    Player player;
    std::vector<std::unique_ptr<Actor>> actors;
    std::vector<std::unique_ptr<StaticObject>> objects;
    std::vector<std::unique_ptr<Item>> items;
    int score;
    bool gameOver;
};
