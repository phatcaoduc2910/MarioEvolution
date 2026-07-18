#pragma once

#include "model/LevelData.h"

#include <SDL2/SDL.h>
#include <string>

/**
 * Cung cấp editor tile có camera và brush theo tutorial của griffpatch.
 *
 * Editor nạp và lưu LevelData qua codec map ký tự dùng chung với game.
 */
class MapEditorService {
public:
    MapEditorService(int mapWidth, int mapHeight, int tileSize,
                     int windowWidth, int windowHeight,
                     std::string mapPath);
    ~MapEditorService();

    MapEditorService(const MapEditorService&) = delete;
    MapEditorService& operator=(const MapEditorService&) = delete;

    bool start();
    void run();

private:
    void handleEvent(const SDL_Event& event);
    void updateCamera();
    bool screenToCell(int screenX, int screenY,
                      int& column, int& row) const;

    void beginStroke();
    void paintStroke();
    void pickTile();
    void selectBrush(int key);
    void generateLevel();
    void saveLevel();

    void render();
    void renderGrid();
    void renderBrushCursor();

    LevelData level;
    std::string mapPath;
    int windowWidth;
    int windowHeight;
    int cameraX;
    int cameraY;
    int mouseX;
    int mouseY;
    TileId selectedTile;
    TileId strokeTile;
    bool editorEnabled;
    bool strokeActive;
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;
};
