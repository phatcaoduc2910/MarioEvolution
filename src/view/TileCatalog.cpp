#include "view/TileCatalog.h"

namespace {
constexpr std::array<TileDefinition, 14> kTileDefinitions{{
    {kEmptyTileId, kEmptyTileId, kManualTileMask,
     nullptr, "EMPTY", false},
    {kStandardBrickTileId, kStandardBrickTileId, kManualTileMask,
     "wood", "BRICK", true},
    {kCoinBrickTileId, kCoinBrickTileId, kManualTileMask,
     "question_1", "COIN BRICK", true},
    {kMushroomBrickTileId, kMushroomBrickTileId, kManualTileMask,
     "question_1", "MUSHROOM", true},
    {kFlowerBrickTileId, kFlowerBrickTileId, kManualTileMask,
     "question_1", "FLOWER", true},
    {kCoinTileId, kCoinTileId, kManualTileMask,
     "coin.1", "COIN", true},
    // Ô đánh dấu chỗ spawn Goomba: World đọc tile này để tạo actor.
    {kGoombaTileId, kGoombaTileId, kManualTileMask,
     "goomba.walk.1", "GOOMBA", true},
    {kKoopaGreenTileId, kKoopaGreenTileId, kManualTileMask,
     "koopa.green.walk.1", "KOOPA", true},
    {kKoopaRedTileId, kKoopaRedTileId, kManualTileMask,
     "koopa.red.walk.1", "KOOPA RED", true},
    {kPiranhaTileId, kPiranhaTileId, kManualTileMask,
     "piranha.plant.1", "PIRANHA", true},
    {kFlagTileId, kFlagTileId, kManualTileMask,
     "flag.1", "FLAG", true},
    {kPlayerSpawnTileId, kPlayerSpawnTileId, kManualTileMask,
     "mario.small.walk.1", "SPAWN", true},
    // Boss arena: 'G' tạo boss, 's' chỉ ghi điểm mọc Piranha cho scheduler.
    {kBossSpawnTileId, kBossSpawnTileId, kManualTileMask,
     "boss.idle.1", "BOSS", true},
    {kPiranhaSpawnPointTileId, kPiranhaSpawnPointTileId, kManualTileMask,
     "piranha.plant.1", "PIRANHA SPOT", true}
}};
}

const std::array<TileDefinition, 14>& tileDefinitions() {
    return kTileDefinitions;
}

const TileDefinition* findTileDefinition(TileId tileId) {
    for (const TileDefinition& definition : kTileDefinitions) {
        if (definition.tileId == tileId) {
            return &definition;
        }
    }
    return nullptr;
}
