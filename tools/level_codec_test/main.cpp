#include "service/LevelCodec.h"

#include "core/Types.h"

#include <array>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

int main() {
    constexpr int kTileSize = 32;
    const std::string sourcePath = "assets/level1.map";
    const std::string copyPath = "builds/tests/level1.roundtrip.map";

    const LevelData source = LevelCodec::load(sourcePath, kTileSize);
    LevelCodec::save(source, copyPath);
    const LevelData copy = LevelCodec::load(copyPath, kTileSize);

    assert(copy.getWidth() == source.getWidth());
    assert(copy.getHeight() == source.getHeight());
    assert(copy.getTileSize() == source.getTileSize());
    assert(copy.getTiles() == source.getTiles());

    std::array<int, 6> tileCounts{};
    for (const TileId tileId : copy.getTiles()) {
        assert(tileId < tileCounts.size());
        ++tileCounts[tileId];
    }
    for (const int count : tileCounts) {
        assert(count > 0);
    }

    LevelData resized(2, 2, kTileSize);
    resized.setTile(0, 0, kStandardBrickTileId);
    resized.setTile(1, 1, kFlowerBrickTileId);
    resized.setPlayerSpawn(1, 1);
    resized.resize(3, 1);
    assert(resized.getWidth() == 3);
    assert(resized.getHeight() == 1);
    assert(resized.getTile(0, 0) == kStandardBrickTileId);
    assert(resized.getTile(2, 0) == kEmptyTileId);
    assert(resized.getSpawnColumn() == 1);
    assert(resized.getSpawnRow() == 0);
    resized.resize(3, 2);
    assert(resized.getTile(1, 1) == kEmptyTileId);

    std::remove(copyPath.c_str());
    std::cout << "Level codec round-trip passed\n";
    return 0;
}
