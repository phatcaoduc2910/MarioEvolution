#include "view/TileCatalog.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>

int main() {
    std::array<bool, 12> seenTileIds{};
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

    assert(paletteTileCount == 11);
    // Brush spawn Goomba phải nằm trong bảng chọn của editor.
    const TileDefinition* goomba = findTileDefinition(kGoombaTileId);
    assert(goomba != nullptr && goomba->paletteVisible);

    for (const TileId actorTileId :
         {kKoopaGreenTileId, kKoopaRedTileId, kPiranhaTileId,
          kFlagTileId, kPlayerSpawnTileId}) {
        const TileDefinition* definition = findTileDefinition(actorTileId);
        assert(definition != nullptr && definition->paletteVisible);
        assert(definition->textureId != nullptr);
    }

    assert(findTileDefinition(999) == nullptr);

    std::cout << "Tile catalog validation passed\n";
    return 0;
}
