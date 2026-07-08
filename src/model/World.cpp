#include "model/World.h"

#include "model/Actor.h"

#include <utility>

World::World()
    : player(0.0, 0.0),
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

void World::update() {}

void World::render() {}
