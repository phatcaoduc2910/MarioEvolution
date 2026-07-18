#include "service/LevelCodec.h"

#include "core/Types.h"

#include <array>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

/**
 * Nạp, lưu lại và so sánh toàn bộ dữ liệu level mẫu.
 *
 * @return 0 khi mọi assertion đều đạt.
 */
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

    std::remove(copyPath.c_str());
    std::cout << "Level codec round-trip passed\n";
    return 0;
}
