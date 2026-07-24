#include "view/TileCatalog.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>

int main() {
    std::array<bool, 6> seenTileIds{};
    std::size_t paletteTileCount = 0;

    for (const TileDefinition& definition : tileDefinitions()) {
        assert(definition.tileId < seenTileIds.size());
        assert(!seenTileIds[definition.tileId]);
        seenTileIds[definition.tileId] = true;
        assert(findTileDefinition(definition.tileId) == &definition);

        const bool validMask = definition.connectionMask == kManualTileMask ||
                               definition.connectionMask <= 0x0f;
        assert(validMask);

        if (definition.paletteVisible) {
            assert(definition.tileId != kEmptyTileId);
            assert(definition.groupId != kEmptyTileId);
            ++paletteTileCount;
        }
    }

    assert(paletteTileCount == 5);
    assert(findTileDefinition(999) == nullptr);

    std::cout << "Tile catalog validation passed\n";
    return 0;
}
