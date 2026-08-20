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
inline constexpr TileId kBossSpawnTileId = 12;
// Điểm mọc Piranha của boss arena: chỉ ghi vị trí, hazard do arena bật ở Phase 3.
inline constexpr TileId kPiranhaSpawnPointTileId = 13;

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

enum class VisualEventType {
    EnemyStomped,
    ShellKicked,
    ShellImpact,
    FireballImpact,
    BrickBroken,
    BossSlamImpact,
    BossDodge
};

// Boss gorilla: HP 6-4 Phase1, 3-2 Phase2 (Enraged), 1 Phase3, 0 Dead.
enum class BossPhase {
    Phase1,
    Phase2,
    Phase3,
    Dead
};

enum class BossAction {
    Idle,
    WindUpThrow,
    Throw,
    Recover,
    Charge,
    GroundSlam,
    Hurt,
    Enraged,
    Dodge,
    Death
};

// Mọi đòn của boss đều đi Prepare (telegraph) -> Active -> Recover.
enum class BossAttackStage {
    Prepare,
    Active,
    Recover
};

enum class BossHitResult {
    Ignored,
    Dodged,
    Damaged
};

struct VisualEvent {
    VisualEventType type;
    double x;
    double y;
    Direction direction;
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