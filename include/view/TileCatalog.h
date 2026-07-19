#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

/**
 * Giá trị dành cho tile chưa tham gia giải thuật tự sắp xếp.
 */
inline constexpr std::uint8_t kManualTileMask = 0xff;

/**
 * Các bit kết nối theo thứ tự trên, phải, dưới và trái.
 */
inline constexpr std::uint8_t kNeighborTop = 1 << 0;
inline constexpr std::uint8_t kNeighborRight = 1 << 1;
inline constexpr std::uint8_t kNeighborBottom = 1 << 2;
inline constexpr std::uint8_t kNeighborLeft = 1 << 3;

/**
 * Mô tả một tile dùng chung giữa renderer, palette và auto-arranging sau này.
 *
 * Các biến thể hình ảnh của cùng một địa hình sẽ dùng chung groupId nhưng có
 * connectionMask khác nhau. Tile chỉ chọn tự động không cần xuất hiện trong
 * palette.
 */
struct TileDefinition {
    TileId tileId;
    TileId groupId;
    std::uint8_t connectionMask;
    SDL_Rect source;
    const char* label;
    bool paletteVisible;
};

const std::array<TileDefinition, 6>& tileDefinitions();
const TileDefinition* findTileDefinition(TileId tileId);
