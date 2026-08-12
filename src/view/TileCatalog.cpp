#include "view/TileCatalog.h"

namespace {
constexpr std::array<TileDefinition, 6> kTileDefinitions{{
    {kEmptyTileId, kEmptyTileId, kManualTileMask,
     {0, 0, 0, 0}, "EMPTY", false},
    {kStandardBrickTileId, kStandardBrickTileId, kManualTileMask,
     {0, 0, 32, 32}, "BRICK", true},
    {kCoinBrickTileId, kCoinBrickTileId, kManualTileMask,
     {32, 0, 32, 32}, "COIN BRICK", true},
    {kMushroomBrickTileId, kMushroomBrickTileId, kManualTileMask,
     {32, 0, 32, 32}, "MUSHROOM", true},
    {kFlowerBrickTileId, kFlowerBrickTileId, kManualTileMask,
     {32, 0, 32, 32}, "FLOWER", true},
    {kCoinTileId, kCoinTileId, kManualTileMask,
     {64, 0, 32, 32}, "COIN", true}
}};
}

const std::array<TileDefinition, 6>& tileDefinitions() {
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
