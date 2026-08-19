#include "service/LevelCodec.h"

#include "core/Types.h"

#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
TileId decodeTile(char symbol) {
    switch (symbol) {
        case '.': return kEmptyTileId;
        case 'B': return kStandardBrickTileId;
        case '?': return kCoinBrickTileId;
        case 'M': return kMushroomBrickTileId;
        case 'F': return kFlowerBrickTileId;
        case 'o': return kCoinTileId;
        case 'g': return kGoombaTileId;
        case 'k': return kKoopaGreenTileId;
        case 'r': return kKoopaRedTileId;
        case 'p': return kPiranhaTileId;
        case 'P': return kPlayerSpawnTileId;
        case 'G': return kBossSpawnTileId;
        case 's': return kPiranhaSpawnPointTileId;
        case '!': return kFlagTileId;
        default:
            throw std::runtime_error(
                std::string("Unknown map symbol: '") + symbol + "'"
            );
    }
}

char encodeTile(TileId tileId) {
    switch (tileId) {
        case kEmptyTileId: return '.';
        case kStandardBrickTileId: return 'B';
        case kCoinBrickTileId: return '?';
        case kMushroomBrickTileId: return 'M';
        case kFlowerBrickTileId: return 'F';
        case kCoinTileId: return 'o';
        case kGoombaTileId: return 'g';
        case kKoopaGreenTileId: return 'k';
        case kKoopaRedTileId: return 'r';
        case kPiranhaTileId: return 'p';
        case kPlayerSpawnTileId: return 'P';
        case kBossSpawnTileId: return 'G';
        case kPiranhaSpawnPointTileId: return 's';
        case kFlagTileId: return '!';
        default:
            throw std::runtime_error(
                "Level contains a tile that has no map symbol"
            );
    }
}
}

LevelData LevelCodec::load(const std::string& path, int tileSize) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open level file: " + path);
    }

    std::vector<std::string> rows;
    std::string line;
    while (std::getline(input, line)) {
        // std::getline giữ lại '\r' khi đọc file CRLF trên Windows.
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            throw std::runtime_error("Level contains an empty row: " + path);
        }
        rows.push_back(line);
    }

    if (rows.empty()) {
        throw std::runtime_error("Level file is empty: " + path);
    }

    const std::size_t width = rows.front().size();
    if (width > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
        rows.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::runtime_error("Level dimensions are too large: " + path);
    }

    LevelData level(
        static_cast<int>(width),
        static_cast<int>(rows.size()),
        tileSize
    );

    bool spawnFound = false;
    for (std::size_t row = 0; row < rows.size(); ++row) {
        if (rows[row].size() != width) {
            throw std::runtime_error("Level rows have different widths: " + path);
        }

        for (std::size_t column = 0; column < width; ++column) {
            const TileId tileId = decodeTile(rows[row][column]);

            // Map chỉ được có một marker 'P'; file thừa marker là map hỏng.
            if (tileId == kPlayerSpawnTileId) {
                if (spawnFound) {
                    throw std::runtime_error(
                        "Level defines more than one 'P' spawn marker: " + path
                    );
                }
                spawnFound = true;
            }

            level.setTile(
                static_cast<int>(column),
                static_cast<int>(row),
                tileId
            );
        }
    }

    return level;
}

void LevelCodec::save(const LevelData& level, const std::string& path) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot write level file: " + path);
    }

    for (int row = 0; row < level.getHeight(); ++row) {
        for (int column = 0; column < level.getWidth(); ++column) {
            output.put(encodeTile(level.getTile(column, row)));
        }
        output.put('\n');
    }

    if (!output) {
        throw std::runtime_error("Failed while writing level file: " + path);
    }
}
