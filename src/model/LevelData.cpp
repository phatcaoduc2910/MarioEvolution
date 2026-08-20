#include "model/LevelData.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

LevelData::LevelData(int width, int height, int tileSize)
    : width(width),
      height(height),
      tileSize(tileSize) {
    if (width <= 0 || height <= 0 || tileSize <= 0) {
        throw std::invalid_argument(
            "Độ dài và kích thước yêu cầu là số dương."
        );
    }

    const std::size_t tileCount =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);
    
    tiles.assign(tileCount, kEmptyTileId);
}

int LevelData::getWidth() const {
    return width;
}

int LevelData::getHeight() const {
    return height;
}

int LevelData::getTileSize() const {
    return tileSize;
}

const std::vector<TileId>& LevelData::getTiles() const {
    return tiles;
}

bool LevelData::isInside(int column, int row) const {
    return column >= 0 && column < width &&
           row >= 0 && row < height;
}

std::size_t LevelData::toIndex(int column, int row) const {
    if (!isInside(column, row)) {
        throw std::out_of_range("Tọa độ ô đang nằm ngoài map.");
    }

    return static_cast<std::size_t>(row) *
        static_cast<std::size_t>(width) +
        static_cast<std::size_t>(column);
}

TileId LevelData::getTile(int column, int row) const {
    return tiles[toIndex(column, row)];
}

void LevelData::setTile(int column, int row, TileId tileId) {
    const std::size_t index = toIndex(column, row);

    // Marker mới thay marker cũ: spawn luôn suy ra từ đúng một ô 'P'.
    if (tileId == kPlayerSpawnTileId) {
        int oldColumn = 0;
        int oldRow = 0;
        if (findPlayerSpawn(oldColumn, oldRow)) {
            tiles[toIndex(oldColumn, oldRow)] = kEmptyTileId;
        }
    }

    tiles[index] = tileId;
}

bool LevelData::findPlayerSpawn(int& column, int& row) const {
    const auto stride = static_cast<std::size_t>(width);
    for (std::size_t index = 0; index < tiles.size(); ++index) {
        if (tiles[index] != kPlayerSpawnTileId) {
            continue;
        }

        column = static_cast<int>(index % stride);
        row = static_cast<int>(index / stride);
        return true;
    }

    return false;
}

void LevelData::resize(int newWidth, int newHeight) {
    if (newWidth <= 0 || newHeight <= 0) {
        throw std::invalid_argument("Kich thuoc map phai la so duong.");
    }

    std::vector<TileId> resized(
        static_cast<std::size_t>(newWidth) *
            static_cast<std::size_t>(newHeight),
        kEmptyTileId);
    const int copyWidth = std::min(width, newWidth);
    const int copyHeight = std::min(height, newHeight);

    // Giữ góc trên-trái; phần nới thêm đã là ô trống.
    for (int row = 0; row < copyHeight; ++row) {
        for (int column = 0; column < copyWidth; ++column) {
            resized[static_cast<std::size_t>(row) * newWidth + column] =
                tiles[static_cast<std::size_t>(row) * width + column];
        }
    }

    width = newWidth;
    height = newHeight;
    tiles = std::move(resized);
}
