#pragma once
#include "core/Types.h"

#include <cstddef>
#include <vector>

class LevelData {
public:
    LevelData(int width, int height, int tileSize);

    int getWidth() const;
    int getHeight() const;
    int getTileSize() const;

    int getSpawnColumn() const;
    int getSpawnRow() const;
    void setPlayerSpawn(int column, int row);

    void resize(int newWidth, int newHeight);

    bool isInside(int column, int row) const;

    TileId getTile(int column, int row) const;
    void setTile(int column, int row, TileId tileId);

    const std::vector<TileId>& getTiles() const;

private:
    std::size_t toIndex(int column, int row) const;

    int width;
    int height;
    int tileSize;
    int spawnColumn;
    int spawnRow;
    std::vector<TileId> tiles;
};
