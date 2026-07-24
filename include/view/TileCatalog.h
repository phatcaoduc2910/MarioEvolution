#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

inline constexpr std::uint8_t kManualTileMask = 0xff;

inline constexpr std::uint8_t kNeighborTop = 1 << 0;
inline constexpr std::uint8_t kNeighborRight = 1 << 1;
inline constexpr std::uint8_t kNeighborBottom = 1 << 2;
inline constexpr std::uint8_t kNeighborLeft = 1 << 3;

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
