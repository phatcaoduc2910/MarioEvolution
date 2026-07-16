#include "service/MapEditorService.h"

#include <SDL2/SDL.h>

#include <iostream>

namespace {
constexpr int kMapWidth = 100;
constexpr int kMapHeight = 40;
constexpr int kTileSize = 32;
constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
}

/**
 * Khởi động SDL2 và chạy map editor độc lập.
 *
 * @return 0 khi editor kết thúc bình thường; 1 khi khởi tạo thất bại.
 */
int main() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Không thể khởi tạo SDL2: " << SDL_GetError() << '\n';
        return 1;
    }

    int exitCode = 0;
    {
        MapEditorService editor(kMapWidth, kMapHeight, kTileSize,
                                kWindowWidth, kWindowHeight);
        if (!editor.start()) {
            exitCode = 1;
        } else {
            editor.run();
        }
    }

    SDL_Quit();
    return exitCode;
}
