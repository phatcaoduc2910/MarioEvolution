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
inline constexpr TileId kStandardBrickTileId = 1;
inline constexpr TileId kCoinBrickTileId = 2;
inline constexpr TileId kMushroomBrickTileId = 3;
inline constexpr TileId kFlowerBrickTileId = 4;
inline constexpr TileId kCoinTileId = 5;

struct Rectangle {
    double x;
    double y;
    int width;
    int height;
};

enum class Direction {
    Left,
    Right
};

enum class PlayerState {
    Small,
    Big,
    Fire,
    Dead
};

enum class PowerUpType {
    None,
    Mushroom,
    FireFlower
};

enum class EnemyState {
    Walking,
    Dead,
    Shell
};

enum class ItemType {
    Coin,
    Mushroom,
    FireFlower
};

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
