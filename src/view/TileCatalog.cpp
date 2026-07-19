#include "view/TileCatalog.h"

namespace {
constexpr std::array<TileDefinition, 6> kTileDefinitions{{
    {kEmptyTileId, kEmptyTileId, kManualTileMask,
     {0, 0, 0, 0}, "EMPTY", false},
    {kStandardBrickTileId, kStandardBrickTileId, kManualTileMask,
     {160, 1104, 16, 16}, "BRICK", true},
    {kCoinBrickTileId, kCoinBrickTileId, kManualTileMask,
     {0, 1120, 16, 16}, "COIN BRICK", true},
    {kMushroomBrickTileId, kMushroomBrickTileId, kManualTileMask,
     {0, 1120, 16, 16}, "MUSHROOM", true},
    {kFlowerBrickTileId, kFlowerBrickTileId, kManualTileMask,
     {0, 1120, 16, 16}, "FLOWER", true},
    {kCoinTileId, kCoinTileId, kManualTileMask,
     {0, 1312, 16, 16}, "COIN", true}
}};
}

/**
 * Lấy toàn bộ định nghĩa tile theo thứ tự ổn định của catalog.
 *
 * @return Mảng định nghĩa tile dùng chung của ứng dụng.
 */
const std::array<TileDefinition, 6>& tileDefinitions() {
    return kTileDefinitions;
}

/**
 * Tìm định nghĩa tương ứng với một tile id.
 *
 * @param tileId Mã tile cần tìm.
 * @return Con trỏ tới định nghĩa tile, hoặc nullptr nếu tile chưa được đăng ký.
 */
const TileDefinition* findTileDefinition(TileId tileId) {
    for (const TileDefinition& definition : kTileDefinitions) {
        if (definition.tileId == tileId) {
            return &definition;
        }
    }
    return nullptr;
}
