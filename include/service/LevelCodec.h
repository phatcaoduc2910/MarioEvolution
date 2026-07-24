#pragma once

#include "model/LevelData.h"

#include <string>

class LevelCodec {
public:
    static LevelData load(const std::string& path, int tileSize);
    static void save(const LevelData& level, const std::string& path);
};
