#pragma once
#include "core/Types.h"

#include <cstddef>
#include <vector>

/**
 * Lưu dữ liệu lưới tile tĩnh của một màn chơi.
 *
 * Lớp chỉ chứa dữ liệu logic của map, không sở hữu tài nguyên SDL hoặc các
 * thực thể đang hoạt động trong World.
 *
 * @todo Phối hợp với view để ánh xạ TileId sang frame trong WorldTiles.png.
 * @todo Phối hợp với CollisionSystem để xác định tile rắn và tránh va chạm
 * trùng với StaticObject.
 */
class LevelData {
public:
    LevelData(int width, int height, int tileSize);

    int getWidth() const;
    int getHeight() const;
    int getTileSize() const;

    int getSpawnColumn() const;
    int getSpawnRow() const;
    void setPlayerSpawn(int column, int row);

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
