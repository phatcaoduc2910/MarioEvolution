#pragma once

#include <cstdint>

/**
 * Mã số xác định một tile địa hình trong lưới map.
 */
using TileId = std::uint16_t;

/**
 * Mã tile dành riêng cho một ô trống trong lưới map.
 */
inline constexpr TileId kEmptyTileId = 0;
/**
 * Tile brick thường, ký hiệu `B` trong file map.
 */
inline constexpr TileId kStandardBrickTileId = 1;
/**
 * Tile coin brick, ký hiệu `?` trong file map.
 */
inline constexpr TileId kCoinBrickTileId = 2;
/**
 * Tile mushroom brick, ký hiệu `M` trong file map.
 */
inline constexpr TileId kMushroomBrickTileId = 3;
/**
 * Tile flower brick, ký hiệu `F` trong file map.
 */
inline constexpr TileId kFlowerBrickTileId = 4;
/**
 * Tile coin tự do, ký hiệu `o` trong file map.
 */
inline constexpr TileId kCoinTileId = 5;

/**
 * Hình chữ nhật song song với các trục dùng cho vùng va chạm gameplay.
 */
struct Rectangle {
    double x;
    double y;
    int width;
    int height;
};

/**
 * Hướng nhìn và hướng chuyển động ngang của actor.
 */
enum class Direction {
    Left,
    Right
};

/**
 * Trạng thái hình thể và sống/chết của player.
 */
enum class PlayerState {
    Small,
    Big,
    Fire,
    Dead
};

/**
 * Năng lực hiện tại do item cấp cho player.
 */
enum class PowerUpType {
    None,
    Mushroom,
    FireFlower
};

/**
 * Trạng thái hành vi chung của enemy.
 */
enum class EnemyState {
    Walking,
    Dead,
    Shell
};

/**
 * Loại nội dung mà special brick có thể chứa.
 */
enum class ItemType {
    Coin,
    Mushroom,
    FireFlower
};

/**
 * Hành động input logic độc lập với mã phím SDL cụ thể.
 */
enum class Key {
    Left,
    Right,
    Up,
    Down,
    Jump,
    Fire,
    Pause,
    Enter
};
