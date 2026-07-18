#include "model/World.h"

#include "model/Actor.h"
#include "model/Brick.h"
#include "model/LevelData.h"

#include <stdexcept>
#include <utility>

/**
 * Khởi tạo World rỗng với player, điểm và trạng thái game ban đầu.
 */
World::World()
    : player(100.0, 502.0),
      score(0),
      gameOver(false) {}

/**
 * @return Tham chiếu có thể chỉnh sửa tới player duy nhất của World.
 */
Player& World::getPlayer() {
    return player;
}

/**
 * @return Danh sách actor động thuộc sở hữu World.
 */
const std::vector<std::unique_ptr<Actor>>& World::getActors() const {
    return actors;
}

/**
 * @return Danh sách object tĩnh thuộc sở hữu World.
 */
const std::vector<std::unique_ptr<StaticObject>>& World::getObjects() const {
    return objects;
}

/**
 * @return Danh sách item thuộc sở hữu World.
 */
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

/**
 * Chuyển quyền sở hữu một actor mới vào World.
 *
 * @param actor Actor được thêm.
 */
void World::addActor(std::unique_ptr<Actor> actor) {
    actors.push_back(std::move(actor));
}

/**
 * Chuyển quyền sở hữu một object tĩnh mới vào World.
 *
 * @param object Object được thêm.
 */
void World::addObject(std::unique_ptr<StaticObject> object) {
    objects.push_back(std::move(object));
}

/**
 * Chuyển quyền sở hữu một item mới vào World.
 *
 * @param item Item được thêm.
 */
void World::addItem(std::unique_ptr<Item> item) {
    items.push_back(std::move(item));
}

/**
 * Cộng một giá trị vào tổng điểm hiện tại.
 *
 * @param points Số điểm cần cộng; giá trị âm sẽ làm giảm tổng điểm.
 */
void World::addScore(int points) {
    score += points;
}

/**
 * @return Tổng điểm hiện tại của World.
 */
int World::getScore() const {
    return score;
}

/**
 * @return true nếu player đã rơi khỏi giới hạn màn; ngược lại là false.
 */
bool World::isGameOver() const {
    return gameOver;
}

/**
 * Cập nhật player, các actor còn sống, item và điều kiện game over.
 */
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
