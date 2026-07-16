#pragma once

#include "model/LevelData.h"

#include <SDL2/SDL.h>

/**
 * Cung cấp editor tile có camera và brush theo tutorial của griffpatch.
 *
 * Editor chỉ thay đổi LevelData trong bộ nhớ. Việc lưu và mở file được dành
 * cho level codec ở bước tiếp theo.
 *
 * @todo Kết nối level codec để lưu và mở LevelData từ file.
 */
class MapEditorService {
public:
    MapEditorService(int mapWidth, int mapHeight, int tileSize,
                     int windowWidth, int windowHeight);
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
    void selectNextBrush(int key);
    void generateLevel();

    void render();
    void renderGrid();
    void renderBrushCursor();

    LevelData level;
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
