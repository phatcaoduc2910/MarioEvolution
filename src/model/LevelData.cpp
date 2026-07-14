#include "model/LevelData.h"

#include <stdexcept>

/**
 * Khởi tạo một màn chơi rỗng với kích thước lưới được chỉ định.
 *
 * Toàn bộ ô trong lưới được gán kEmptyTileId và vị trí sinh của người chơi
 * được đặt tại ô đầu tiên.
 *
 * @param width Số cột tile của màn chơi.
 * @param height Số hàng tile của màn chơi.
 * @param tileSize Chiều rộng và chiều cao của một tile, tính bằng pixel.
 * @throws std::invalid_argument Khi width, height hoặc tileSize không dương.
 */
LevelData::LevelData(int width, int height, int tileSize)
    : width(width),
      height(height),
      tileSize(tileSize),
      spawnColumn(0),
      spawnRow(0) {
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

/**
 * Lấy số cột tile của màn chơi.
 *
 * @return Chiều rộng lưới tính theo số tile.
 */
int LevelData::getWidth() const {
    return width;
}

/**
 * Lấy số hàng tile của màn chơi.
 *
 * @return Chiều cao lưới tính theo số tile.
 */
int LevelData::getHeight() const {
    return height;
}

/**
 * Lấy kích thước cạnh của một tile.
 *
 * @return Kích thước tile tính bằng pixel.
 */
int LevelData::getTileSize() const {
    return tileSize;
}

/**
 * Lấy cột sinh của người chơi trong lưới tile.
 *
 * @return Chỉ số cột sinh của người chơi.
 */
int LevelData::getSpawnColumn() const {
    return spawnColumn;
}

/**
 * Lấy hàng sinh của người chơi trong lưới tile.
 *
 * @return Chỉ số hàng sinh của người chơi.
 */
int LevelData::getSpawnRow() const {
    return spawnRow;
}

/**
 * Lấy toàn bộ tile theo thứ tự từ trái sang phải, từ trên xuống dưới.
 *
 * @return Tham chiếu chỉ đọc tới danh sách tile của màn chơi.
 */
const std::vector<TileId>& LevelData::getTiles() const {
    return tiles;
}

/**
 * Kiểm tra một tọa độ ô có nằm trong lưới tile hay không.
 *
 * @param column Chỉ số cột cần kiểm tra.
 * @param row Chỉ số hàng cần kiểm tra.
 * @return true nếu ô nằm trong lưới; ngược lại là false.
 */
bool LevelData::isInside(int column, int row) const {
    return column >= 0 && column < width &&
           row >= 0 && row < height;
}

/**
 * Chuyển tọa độ cột, hàng thành vị trí trong vector tile dạng row-major.
 *
 * @param column Chỉ số cột của tile.
 * @param row Chỉ số hàng của tile.
 * @return Vị trí tương ứng trong vector tile.
 * @throws std::out_of_range Khi tọa độ nằm ngoài lưới.
 */
std::size_t LevelData::toIndex(int column, int row) const {
    if (!isInside(column, row)) {
        throw std::out_of_range("Tọa độ ô đang nằm ngoài map.");
    }

    return static_cast<std::size_t>(row) *
        static_cast<std::size_t>(width) +
        static_cast<std::size_t>(column);
}

/**
 * Lấy mã tile tại một ô trong lưới.
 *
 * @param column Chỉ số cột của tile.
 * @param row Chỉ số hàng của tile.
 * @return Mã tile đang được lưu tại ô.
 * @throws std::out_of_range Khi tọa độ nằm ngoài lưới.
 */
TileId LevelData::getTile(int column, int row) const {
    return tiles[toIndex(column, row)];
}

/**
 * Gán mã tile mới cho một ô trong lưới.
 *
 * @param column Chỉ số cột của tile.
 * @param row Chỉ số hàng của tile.
 * @param tileId Mã tile mới cần lưu.
 * @throws std::out_of_range Khi tọa độ nằm ngoài lưới.
 */
void LevelData::setTile(int column, int row, TileId tileId) {
    tiles[toIndex(column, row)] = tileId;
}

/**
 * Đặt ô sinh của người chơi trong lưới tile.
 *
 * @param column Chỉ số cột sinh của người chơi.
 * @param row Chỉ số hàng sinh của người chơi.
 * @throws std::out_of_range Khi tọa độ nằm ngoài lưới.
 */
void LevelData::setPlayerSpawn(int column, int row) {
    if (!isInside(column, row)) {
        throw std::out_of_range("Người chơi đang sinh ra ở ngoài map.");
    }

    spawnColumn = column;
    spawnRow = row;
}
