#pragma once

#include <cstdint>

using TileId = std::uint16_t;

inline constexpr TileId kEmptyTileId = 0;
inline constexpr TileId kStandardBrickTileId = 1;
inline constexpr TileId kCoinBrickTileId = 2;
inline constexpr TileId kMushroomBrickTileId = 3;
inline constexpr TileId kFlowerBrickTileId = 4;
inline constexpr TileId kCoinTileId = 5;
inline constexpr TileId kGoombaTileId = 6;
inline constexpr TileId kFlagTileId = 7;
inline constexpr TileId kKoopaGreenTileId = 8;
inline constexpr TileId kKoopaRedTileId = 9;
inline constexpr TileId kPiranhaTileId = 10;
inline constexpr TileId kPlayerSpawnTileId = 11;

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
    Shell,
    ShellSliding
};

enum class KoopaColor {
    Green,
    Red
};

enum class PiranhaPhase {
    Hidden,
    Rising,
    Exposed,
    Sinking
};

enum class ItemType {
    Coin,
    Mushroom,
    FireFlower
};

enum class Key {
    None,
    Left,
    Right,
    Up,
    Down,
    Jump,
    Fire,
    Esc,
    Enter,
    Edit
};

enum GameState {
    StartMenu,
    Playing,
    Paused,
    Editing,
    LevelComplete,
    GameOver,
    Exit
};

enum class Option {
    None,          
    StartGame,      
    ExitGame,          
};