#pragma once

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
