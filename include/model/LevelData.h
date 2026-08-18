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

    // Marker 'P' trong lưới tile là nguồn spawn duy nhất; trả về false khi
    // map chưa có marker nào.
    bool findPlayerSpawn(int& column, int& row) const;

    void resize(int newWidth, int newHeight);

    bool isInside(int column, int row) const;

    TileId getTile(int column, int row) const;
    // Đặt marker 'P' sẽ xoá marker cũ để map không bao giờ có hai chỗ spawn.
    void setTile(int column, int row, TileId tileId);

    const std::vector<TileId>& getTiles() const;

private:
    std::size_t toIndex(int column, int row) const;

    int width;
    int height;
    int tileSize;
    std::vector<TileId> tiles;
};
