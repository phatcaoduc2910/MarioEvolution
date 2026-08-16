#include "view/TileCatalog.h"

namespace {
constexpr std::array<TileDefinition, 7> kTileDefinitions{{
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
     "goomba.walk.1", "GOOMBA", true}
}};
}

const std::array<TileDefinition, 7>& tileDefinitions() {
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
