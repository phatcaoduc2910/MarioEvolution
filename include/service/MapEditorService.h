#pragma once

#include "model/LevelData.h"

#include <SDL2/SDL.h>
#include <cstddef>
#include <string>
#include <vector>

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
    enum class PalettePage {
        Tiles,
        Maps
    };

    void updateCamera();
    int getViewportWidth() const;
    int getVisibleMapRows() const;
    bool screenToCell(int screenX, int screenY,
                      int& column, int& row) const;
    bool paletteTileAt(int screenX, int screenY, TileId& tileId) const;
    bool handlePanelClick(int screenX, int screenY);

    void beginStroke();
    void paintStroke();
    void pickTile();
    void selectBrush(int paletteNumber);
    void generateLevel();
    void createNewLevel();
    void resizeLevel(int newWidth, int newHeight);
    void refreshSavedMaps();
    void openSavedMap(std::size_t index);
    void beginSaveAs();
    bool handleSaveAsEvent(const SDL_Event& event);
    void saveLevelAs();
    bool canDiscardChanges(const std::string& target);
    void saveLevel();

    void renderGrid(SDL_Renderer* renderer, SDL_Texture* worldTiles);
    void renderBrushCursor(SDL_Renderer* renderer, SDL_Texture* worldTiles);
    void renderPalette(SDL_Renderer* renderer, SDL_Texture* worldTiles);
    void renderMapPalette(SDL_Renderer* renderer);

    LevelData level;
    std::string mapPath;
    std::string mapDirectory;
    std::vector<std::string> savedMaps;
    std::string saveAsName;
    std::string pendingDiscardTarget;
    std::string statusMessage;
    int windowWidth;
    int windowHeight;
    int cameraX;
    int cameraY;
    int mouseX;
    int mouseY;
    int mapListOffset;
    TileId selectedTile;
    TileId strokeTile;
    PalettePage palettePage;
    bool editorEnabled;
    bool dirty;
    bool strokeActive;
    bool namingMap;
};
