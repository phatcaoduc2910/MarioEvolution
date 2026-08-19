#include "service/LevelCodec.h"

#include "core/Types.h"

#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    constexpr int kTileSize = 32;
    const std::string sourcePath = "assets/maps/normal_1.map";
    const std::string copyPath = "builds/tests/normal_1.roundtrip.map";

    const LevelData source = LevelCodec::load(sourcePath, kTileSize);
    LevelCodec::save(source, copyPath);
    const LevelData copy = LevelCodec::load(copyPath, kTileSize);

    assert(copy.getWidth() == source.getWidth());
    assert(copy.getHeight() == source.getHeight());
    assert(copy.getTileSize() == source.getTileSize());
    assert(copy.getTiles() == source.getTiles());

    std::array<int, 14> tileCounts{};
    for (const TileId tileId : copy.getTiles()) {
        assert(tileId < tileCounts.size());
        ++tileCounts[tileId];
    }
    for (TileId tileId = kEmptyTileId; tileId <= kCoinTileId; ++tileId) {
        assert(tileCounts[tileId] > 0);
    }

    int spawnColumn = 0;
    int spawnRow = 0;

    LevelData resized(2, 2, kTileSize);
    resized.setTile(0, 0, kStandardBrickTileId);
    resized.setTile(1, 1, kFlowerBrickTileId);
    resized.setTile(1, 0, kPlayerSpawnTileId);
    resized.resize(3, 1);
    assert(resized.getWidth() == 3);
    assert(resized.getHeight() == 1);
    assert(resized.getTile(0, 0) == kStandardBrickTileId);
    assert(resized.getTile(2, 0) == kEmptyTileId);
    // Marker còn nằm trong vùng giữ lại nên spawn không đổi ô.
    assert(resized.findPlayerSpawn(spawnColumn, spawnRow));
    assert(spawnColumn == 1);
    assert(spawnRow == 0);
    resized.resize(3, 2);
    assert(resized.getTile(1, 1) == kEmptyTileId);

    // Marker mới dời spawn và xoá marker cũ: map chỉ còn một chỗ spawn.
    resized.setTile(2, 1, kPlayerSpawnTileId);
    assert(resized.getTile(1, 0) == kEmptyTileId);
    assert(resized.findPlayerSpawn(spawnColumn, spawnRow));
    assert(spawnColumn == 2);
    assert(spawnRow == 1);

    // Map cắt mất marker thì báo là không có spawn, không đoán bừa ô khác.
    resized.resize(2, 1);
    assert(!resized.findPlayerSpawn(spawnColumn, spawnRow));

    const std::string actorSymbolPath =
        "builds/tests/level1.actor_symbols.map";
    const std::string actorCopyPath =
        "builds/tests/level1.actor_symbols.roundtrip.map";
    const std::string unknownSymbolPath =
        "builds/tests/level1.unknown_symbol.map";
    const std::string inconsistentWidthPath =
        "builds/tests/level1.inconsistent_width.map";
    const std::string duplicateSpawnPath =
        "builds/tests/level1.duplicate_spawn.map";
    const std::string missingSpawnPath =
        "builds/tests/level1.missing_spawn.map";

    const auto writeFixture = [](const std::string& path, const char* contents) {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        assert(output);
        output << contents;
        output.close();
        assert(output);
    };
    writeFixture(actorSymbolPath, "g!\nkr\npP\n");
    writeFixture(unknownSymbolPath, "...\n.Z.\n");
    writeFixture(inconsistentWidthPath, "...\n..\n");
    writeFixture(duplicateSpawnPath, "P..\n..P\n");
    writeFixture(missingSpawnPath, "...\n...\n");

    const LevelData actorSymbols = LevelCodec::load(actorSymbolPath, kTileSize);
    assert(actorSymbols.getTile(0, 0) == kGoombaTileId);
    assert(actorSymbols.getTile(1, 0) == kFlagTileId);
    assert(actorSymbols.getTile(0, 1) == kKoopaGreenTileId);
    assert(actorSymbols.getTile(1, 1) == kKoopaRedTileId);
    assert(actorSymbols.getTile(0, 2) == kPiranhaTileId);
    assert(actorSymbols.getTile(1, 2) == kPlayerSpawnTileId);
    assert(actorSymbols.findPlayerSpawn(spawnColumn, spawnRow));
    assert(spawnColumn == 1);
    assert(spawnRow == 2);
    LevelCodec::save(actorSymbols, actorCopyPath);
    assert(LevelCodec::load(actorCopyPath, kTileSize).getTiles() ==
           actorSymbols.getTiles());

    const auto expectRuntimeError = [kTileSize](const std::string& path) {
        bool threwRuntimeError = false;
        try {
            (void)LevelCodec::load(path, kTileSize);
        } catch (const std::runtime_error&) {
            threwRuntimeError = true;
        }
        assert(threwRuntimeError);
    };
    expectRuntimeError(unknownSymbolPath);
    expectRuntimeError(inconsistentWidthPath);
    // Hai marker 'P' là map hỏng, không được im lặng chọn cái đầu.
    expectRuntimeError(duplicateSpawnPath);

    // Map cũ chưa có marker vẫn đọc được, chỉ là không có spawn.
    const LevelData missingSpawn = LevelCodec::load(missingSpawnPath, kTileSize);
    assert(!missingSpawn.findPlayerSpawn(spawnColumn, spawnRow));

    std::remove(copyPath.c_str());
    std::remove(actorSymbolPath.c_str());
    std::remove(actorCopyPath.c_str());
    std::remove(unknownSymbolPath.c_str());
    std::remove(inconsistentWidthPath.c_str());
    std::remove(duplicateSpawnPath.c_str());
    std::remove(missingSpawnPath.c_str());
    std::cout << "Level codec round-trip passed\n";
    return 0;
}
