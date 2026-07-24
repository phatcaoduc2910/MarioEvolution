#include "service/LevelCodec.h"
#include "service/MapEditorService.h"

#include "core/Types.h"

#include <SDL2/SDL.h>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>

namespace {
SDL_Event keyDown(SDL_Keycode keycode, SDL_Keymod modifiers = KMOD_NONE) {
    SDL_Event event{};
    event.type = SDL_KEYDOWN;
    event.key.type = SDL_KEYDOWN;
    event.key.repeat = 0;
    event.key.keysym.sym = keycode;
    event.key.keysym.mod = modifiers;
    return event;
}

SDL_Event click(int x, int y) {
    SDL_Event event{};
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.type = SDL_MOUSEBUTTONDOWN;
    event.button.button = SDL_BUTTON_LEFT;
    event.button.x = x;
    event.button.y = y;
    return event;
}

SDL_Event textInput(const std::string& text) {
    SDL_Event event{};
    event.type = SDL_TEXTINPUT;
    event.text.type = SDL_TEXTINPUT;
    std::snprintf(event.text.text, sizeof(event.text.text), "%s", text.c_str());
    return event;
}
}

int main() {
    constexpr int kTileSize = 32;
    constexpr int kWindowWidth = 800;
    constexpr int kWindowHeight = 600;
    const std::filesystem::path directory =
        "builds/tests/map_editor_files";
    const std::filesystem::path seedPath = directory / "seed.map";

    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);

    LevelData seed(6, 6, kTileSize);
    for (int column = 0; column < seed.getWidth(); ++column) {
        seed.setTile(column, seed.getHeight() - 1, kStandardBrickTileId);
    }
    LevelCodec::save(seed, seedPath.string());

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    {
        MapEditorService editor(
            6, 6, kTileSize, kWindowWidth, kWindowHeight,
            seedPath.string());

        editor.handleEvent(keyDown(SDLK_0));

        editor.handleEvent(click(700, 55));
        editor.handleEvent(click(650, 181));
        assert(editor.getLevel().getWidth() == 7);
        assert(editor.getLevel().getHeight() == 6);

        // Resize làm map dirty, nên lần NEW đầu chỉ bật cảnh báo.
        editor.handleEvent(click(610, 223));
        editor.handleEvent(click(610, 223));
        assert(editor.getLevel().getWidth() == 7);
        for (int column = 0; column < editor.getLevel().getWidth(); ++column) {
            assert(editor.getLevel().getTile(
                       column, editor.getLevel().getHeight() - 1) ==
                   kStandardBrickTileId);
        }

        editor.handleEvent(click(704, 223));
        editor.handleEvent(textInput("new_map"));
        editor.handleEvent(keyDown(SDLK_RETURN));
        assert(std::filesystem::exists(directory / "new_map.map"));

        editor.handleEvent(keyDown(SDLK_F2));
        editor.handleEvent(textInput("second"));
        editor.handleEvent(keyDown(SDLK_RETURN));
        assert(std::filesystem::exists(directory / "second.map"));

        LevelData external(9, 6, kTileSize);
        LevelCodec::save(external, (directory / "aaa.map").string());
        // F5 phải nhặt được file vừa xuất hiện bên ngoài editor.
        editor.handleEvent(keyDown(SDLK_F5));
        editor.handleEvent(click(610, 331));
        assert(editor.getLevel().getWidth() == 9);
        assert(editor.getLevel().getHeight() == 6);
    }

    SDL_Quit();
    std::filesystem::remove_all(directory, error);
    std::cout << "Map editor file workflow passed\n";
    return 0;
}
