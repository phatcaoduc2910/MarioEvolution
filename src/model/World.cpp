#include "model/World.h"

#include "model/Actor.h"

#include <SDL2/SDL.h>
#include <utility>

World::World()
    : player(100.0, 502.0),
      score(0),
      gameOver(false) {
    // Nền đất
    addObject(std::make_unique<StaticObject>(
        0.0, 550.0, 800, 50
    ));

    // Bệ 
    addObject(std::make_unique<StaticObject>(
        320.0, 500.0, 160, 32
    ));
}


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

// Viết tạm hàm renderer để thay cho việc render trong view. Sau này sẽ xóa.
void World::render(SDL_Renderer* renderer) const {
    if (renderer == nullptr) {
        return;
    }

    // Render nền đất và bệ.
    SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);
    for (const auto& object : objects) {
        const SDL_Rect destination{
            static_cast<int>(object->getX()),
            static_cast<int>(object->getY()),
            object->getWidth(),
            object->getHeight()
        };
        SDL_RenderFillRect(renderer, &destination);
    }

    // Render player.
    const SDL_Rect playerDestination{
        static_cast<int>(player.getX()),
        static_cast<int>(player.getY()),
        player.getWidth(),
        player.getHeight()
    };
    SDL_SetRenderDrawColor(renderer, 220, 45, 45, 255);
    SDL_RenderFillRect(renderer, &playerDestination);
}
