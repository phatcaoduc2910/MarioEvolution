#pragma once

#include "model/LevelData.h"

#include <string>

/**
 * Đọc và ghi LevelData bằng lưới ký tự đơn giản.
 */
class LevelCodec {
public:
    static LevelData load(const std::string& path, int tileSize);
    static void save(const LevelData& level, const std::string& path);
};
