#include "model/Brick.h"

#include "model/Item.h"
#include "model/Player.h"

#include <algorithm>

/**
 * Khởi tạo brick 32x32 với trạng thái chưa mở.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 * @param breakable Cho biết brick có thể bị phá hay không.
 */
Brick::Brick(double x, double y, bool breakable)
    : StaticObject(x, y, 32, 32),
      breakable(breakable),
      opened(false) {}

/**
 * Xử lý khi player đập brick từ phía dưới.
 *
 * Brick thường chỉ bị phá khi player không ở trạng thái Small hoặc Dead. Nội
 * dung của brick đặc biệt được xử lý riêng bởi releaseItem().
 *
 * @param player Player tác động vào brick.
 */
void Brick::hitBy(Player& player) {
    if (opened) {
        return;
    }

    if (breakable) {
        if (player.getState() != PlayerState::Small &&
            player.getState() != PlayerState::Dead) {
            opened = true;
            solid = false;
        }
        return;
    }

    // Gạch đặc biệt được mở bởi releaseItem(); hàm này chuyển quyền sở hữu
    // vật phẩm vừa tạo cho bên gọi.
}

/**
 * @return true nếu brick cho phép bị phá; ngược lại là false.
 */
bool Brick::canBeBroken() const {
    return breakable;
}

/**
 * @return true nếu brick đã được mở hoặc phá; ngược lại là false.
 */
bool Brick::isOpened() const {
    return opened;
}

/**
 * Khởi tạo một brick thường có thể bị phá.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 */
StandardBrick::StandardBrick(double x, double y)
    : Brick(x, y, true) {}

/**
 * Phá brick thường và vô hiệu hóa va chạm rắn của nó.
 */
void StandardBrick::breakBrick() {
    if (!opened) {
        opened = true;
        solid = false;
    }
}

/**
 * Khởi tạo brick đặc biệt chứa một loại item.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 * @param content Loại item được chứa trong brick.
 */
SpecialBrick::SpecialBrick(double x, double y, ItemType content)
    : Brick(x, y, false),
      content(content) {}

/**
 * Tạo item nằm phía trên brick nếu brick chưa được mở.
 *
 * @return Quyền sở hữu item vừa tạo, hoặc nullptr nếu brick đã mở.
 */
std::unique_ptr<Item> SpecialBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    std::unique_ptr<Item> item;
    switch (content) {
        case ItemType::Coin:
            item = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
            break;
        case ItemType::Mushroom:
            item = std::make_unique<Mushroom>(x, y - 32);
            break;
        case ItemType::FireFlower:
            item = std::make_unique<FireFlower>(x, y - 32);
            break;
    }

    open();
    return item;
}

/**
 * Đánh dấu brick đặc biệt đã được mở.
 */
void SpecialBrick::open() {
    opened = true;
}

/**
 * Khởi tạo coin brick với số coin không âm.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 * @param coinAmount Số coin có thể giải phóng.
 */
CoinBrick::CoinBrick(double x, double y, int coinAmount)
    : SpecialBrick(x, y, ItemType::Coin),
      coinAmount(std::max(0, coinAmount)) {}

/**
 * Giải phóng một coin và giảm số coin còn lại.
 *
 * @return Quyền sở hữu coin mới, hoặc nullptr khi brick đã hết coin.
 */
std::unique_ptr<Item> CoinBrick::releaseItem() {
    if (coinAmount <= 0) {
        open();
        return nullptr;
    }

    auto coin = std::make_unique<Coin>(x + (width - 16) / 2.0, y - 16, 1);
    --coinAmount;
    if (coinAmount == 0) {
        open();
    }
    return coin;
}

/**
 * Khởi tạo brick chứa mushroom.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 */
MushroomBrick::MushroomBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::Mushroom) {}

/**
 * Giải phóng mushroom ở lần mở đầu tiên.
 *
 * @return Quyền sở hữu mushroom mới, hoặc nullptr nếu brick đã mở.
 */
std::unique_ptr<Item> MushroomBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    auto mushroom = std::make_unique<Mushroom>(x, y - 32);
    open();
    return mushroom;
}

/**
 * Khởi tạo brick chứa fire flower.
 *
 * @param x Tọa độ x của brick.
 * @param y Tọa độ y của brick.
 */
FlowerBrick::FlowerBrick(double x, double y)
    : SpecialBrick(x, y, ItemType::FireFlower) {}

/**
 * Giải phóng fire flower ở lần mở đầu tiên.
 *
 * @return Quyền sở hữu fire flower mới, hoặc nullptr nếu brick đã mở.
 */
std::unique_ptr<Item> FlowerBrick::releaseItem() {
    if (opened) {
        return nullptr;
    }

    auto flower = std::make_unique<FireFlower>(x, y - 32);
    open();
    return flower;
}
