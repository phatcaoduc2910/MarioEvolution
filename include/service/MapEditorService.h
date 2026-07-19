#pragma once

#include "model/LevelData.h"

#include <SDL2/SDL.h>
#include <string>

/**
 * Cung cấp editor tile nhúng trực tiếp trong cửa sổ game.
 *
 * Editor sở hữu LevelData, xử lý palette/camera/input và nạp lưu map qua codec.
 * Game vẫn sở hữu SDL window, renderer và texture được truyền vào khi render.
 */
class MapEditorService {
public:
    MapEditorService(int mapWidth, int mapHeight, int tileSize,
                     int windowWidth, int windowHeight,
                     std::string mapPath);

    MapEditorService(const MapEditorService&) = delete;
    MapEditorService& operator=(const MapEditorService&) = delete;

    bool isEnabled() const;
    bool isDirty() const;
    bool handleEvent(const SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer, SDL_Texture* worldTiles);

    const LevelData& getLevel() const;
    int getCameraX() const;
    int getCameraY() const;
    SDL_Rect getMapViewport() const;

private:
    void updateCamera();
    int getViewportWidth() const;
    bool screenToCell(int screenX, int screenY,
                      int& column, int& row) const;
    bool paletteTileAt(int screenX, int screenY, TileId& tileId) const;

    void beginStroke();
    void paintStroke();
    void pickTile();
    void selectBrush(int paletteNumber);
    void generateLevel();
    void saveLevel();

    void renderGrid(SDL_Renderer* renderer, SDL_Texture* worldTiles);
    void renderBrushCursor(SDL_Renderer* renderer, SDL_Texture* worldTiles);
    void renderPalette(SDL_Renderer* renderer, SDL_Texture* worldTiles);

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
    bool dirty;
    bool strokeActive;
};
