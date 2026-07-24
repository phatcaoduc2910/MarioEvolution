#include "service/MapEditorService.h"

#include "service/LevelCodec.h"
#include "view/TileCatalog.h"
#include "view/UiRenderer.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <system_error>
#include <utility>

namespace {
constexpr int kCameraSpeed = 12;
constexpr int kPaletteWidth = 208;
constexpr int kPalettePadding = 12;
constexpr int kPaletteTop = 104;
constexpr int kPaletteColumns = 2;
constexpr int kPaletteCellWidth = 92;
constexpr int kPaletteCellHeight = 88;
constexpr int kPalettePreviewSize = 48;
constexpr int kTabTop = 48;
constexpr int kTabWidth = 84;
constexpr int kTabHeight = 26;
constexpr int kMapButtonsTop = 176;
constexpr int kMapActionTop = 218;
constexpr int kMapSaveAsTop = 258;
constexpr int kMapListTop = 326;
constexpr int kMapListRowHeight = 30;
constexpr int kMinMapDimension = 5;
constexpr int kMaxMapDimension = 200;

constexpr SDL_Color kMapColor{100, 149, 237, 255};
constexpr SDL_Color kPanelColor{28, 32, 40, 255};
constexpr SDL_Color kCardColor{47, 54, 66, 255};
constexpr SDL_Color kGridColor{65, 86, 112, 150};
constexpr SDL_Color kTextColor{238, 242, 248, 255};
constexpr SDL_Color kMutedTextColor{166, 177, 194, 255};
constexpr SDL_Color kSelectedColor{255, 181, 46, 255};

void setDrawColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

const TileDefinition* paletteDefinition(std::size_t index) {
    std::size_t visibleIndex = 0;
    for (const TileDefinition& definition : tileDefinitions()) {
        if (!definition.paletteVisible) {
            continue;
        }
        if (visibleIndex == index) {
            return &definition;
        }
        ++visibleIndex;
    }
    return nullptr;
}

bool renderTile(SDL_Renderer* renderer, SDL_Texture* texture, TileId tileId,
                const SDL_Rect& destination, Uint8 alpha = 255) {
    const TileDefinition* definition = findTileDefinition(tileId);
    if (renderer == nullptr || texture == nullptr || definition == nullptr ||
        definition->source.w <= 0 || definition->source.h <= 0) {
        return false;
    }

    Uint8 oldAlpha = 255;
    SDL_GetTextureAlphaMod(texture, &oldAlpha);
    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_RenderCopy(renderer, texture, &definition->source, &destination);
    // Preview mượn alpha của texture, xong phải trả lại cho lượt vẽ sau.
    SDL_SetTextureAlphaMod(texture, oldAlpha);
    return true;
}

std::string mapName(const std::string& path) {
    return std::filesystem::path(path).stem().string();
}

bool contains(const SDL_Rect& rectangle, int x, int y) {
    return x >= rectangle.x && x < rectangle.x + rectangle.w &&
           y >= rectangle.y && y < rectangle.y + rectangle.h;
}

std::string fitLabel(const std::string& text, std::size_t length) {
    if (text.size() <= length) {
        return text;
    }
    return text.substr(0, length);
}

std::string fitTailLabel(const std::string& text, std::size_t length) {
    if (text.size() <= length) {
        return text;
    }
    return text.substr(text.size() - length);
}

void renderButton(SDL_Renderer* renderer, const SDL_Rect& rectangle,
                  const char* label, bool selected = false) {
    UiRenderer::fillRect(
        renderer, rectangle, selected ? kSelectedColor : kCardColor);
    UiRenderer::drawText(
        renderer, label, rectangle.x + 7, rectangle.y + 9, 1,
        selected ? kPanelColor : kTextColor);
}
}

MapEditorService::MapEditorService(int mapWidth, int mapHeight, int tileSize,
                                   int windowWidth, int windowHeight,
                                   std::string mapPath)
    : level(mapWidth, mapHeight, tileSize),
      mapPath(std::move(mapPath)),
      mapDirectory(std::filesystem::path(this->mapPath).parent_path().string()),
      windowWidth(windowWidth),
      windowHeight(windowHeight),
      cameraX(0),
      cameraY(0),
      mouseX(0),
      mouseY(0),
      mapListOffset(0),
      selectedTile(kStandardBrickTileId),
      strokeTile(kEmptyTileId),
      palettePage(PalettePage::Tiles),
      editorEnabled(false),
      dirty(false),
      strokeActive(false),
      namingMap(false) {
    if (mapDirectory.empty()) {
        mapDirectory = ".";
    }

    try {
        level = LevelCodec::load(this->mapPath, tileSize);
        std::cout << "Loaded map: " << this->mapPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\nCreating a new map instead.\n";
        generateLevel();
    }
    refreshSavedMaps();
}


bool MapEditorService::isEnabled() const {
    return editorEnabled;
}


bool MapEditorService::isDirty() const {
    return dirty;
}

bool MapEditorService::handleEvent(const SDL_Event& event) {
    if (!editorEnabled) {
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            event.key.keysym.sym == SDLK_0) {
            editorEnabled = true;
            strokeActive = false;
            refreshSavedMaps();
            return true;
        }
        return false;
    }

    if (namingMap) {
        return handleSaveAsEvent(event);
    }

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
        event.key.keysym.sym == SDLK_0) {
        editorEnabled = false;
        strokeActive = false;
        return true;
    }

    if (event.type == SDL_MOUSEMOTION) {
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
        mouseX = event.button.x;
        mouseY = event.button.y;

        if (handlePanelClick(mouseX, mouseY)) {
            strokeActive = false;
            return true;
        }

        TileId paletteTile = kEmptyTileId;
        if (paletteTileAt(mouseX, mouseY, paletteTile)) {
            selectedTile = paletteTile;
            strokeActive = false;
        } else {
            beginStroke();
        }
        return true;
    }

    if (event.type == SDL_MOUSEWHEEL && palettePage == PalettePage::Maps) {
        const int maxOffset = std::max(
            0, static_cast<int>(savedMaps.size()) - getVisibleMapRows());
        mapListOffset = std::clamp(
            mapListOffset - event.wheel.y, 0, maxOffset);
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
        strokeActive = false;
        return true;
    }

    if (event.type == SDL_KEYUP) {
        return false;
    }

    if (event.type != SDL_KEYDOWN) {
        return true;
    }

    if (event.key.repeat != 0) {
        return true;
    }

    const SDL_Keycode key = event.key.keysym.sym;
    if (key == SDLK_ESCAPE) {
        editorEnabled = false;
        strokeActive = false;
    } else if (key == SDLK_TAB) {
        palettePage = palettePage == PalettePage::Tiles
                          ? PalettePage::Maps
                          : PalettePage::Tiles;
        if (palettePage == PalettePage::Maps) {
            refreshSavedMaps();
        }
    } else if (key == SDLK_n) {
        createNewLevel();
    } else if (key == SDLK_F2 ||
               (key == SDLK_s &&
                (event.key.keysym.mod & KMOD_CTRL) != 0 &&
                (event.key.keysym.mod & KMOD_SHIFT) != 0)) {
        beginSaveAs();
    } else if (key >= SDLK_1 && key <= SDLK_5) {
        selectBrush(static_cast<int>(key - SDLK_0));
    } else if (key == SDLK_e) {
        pickTile();
    } else if (key == SDLK_r) {
        generateLevel();
    } else if (key == SDLK_LEFTBRACKET) {
        resizeLevel(level.getWidth() - 1, level.getHeight());
    } else if (key == SDLK_RIGHTBRACKET) {
        resizeLevel(level.getWidth() + 1, level.getHeight());
    } else if (key == SDLK_MINUS) {
        resizeLevel(level.getWidth(), level.getHeight() - 1);
    } else if (key == SDLK_EQUALS) {
        resizeLevel(level.getWidth(), level.getHeight() + 1);
    } else if (key == SDLK_F5) {
        refreshSavedMaps();
    } else if (key == SDLK_s &&
               (event.key.keysym.mod & KMOD_CTRL) != 0) {
        saveLevel();
    }
    return true;
}

void MapEditorService::update() {
    if (editorEnabled && !namingMap) {
        updateCamera();
        if (strokeActive) {
            paintStroke();
        }
    }
}

void MapEditorService::render(SDL_Renderer* renderer,
                              SDL_Texture* worldTiles) {
    if (!editorEnabled || renderer == nullptr || worldTiles == nullptr) {
        return;
    }

    setDrawColor(renderer, kMapColor);
    SDL_RenderClear(renderer);

    const SDL_Rect viewport = getMapViewport();
    SDL_RenderSetClipRect(renderer, &viewport);
    renderGrid(renderer, worldTiles);
    renderBrushCursor(renderer, worldTiles);
    SDL_RenderSetClipRect(renderer, nullptr);
    renderPalette(renderer, worldTiles);
}


const LevelData& MapEditorService::getLevel() const {
    return level;
}


int MapEditorService::getCameraX() const {
    return cameraX;
}


int MapEditorService::getCameraY() const {
    return cameraY;
}


SDL_Rect MapEditorService::getMapViewport() const {
    return {0, 0, getViewportWidth(), windowHeight};
}

void MapEditorService::updateCamera() {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        cameraX -= kCameraSpeed;
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        cameraX += kCameraSpeed;
    }
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        cameraY -= kCameraSpeed;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        cameraY += kCameraSpeed;
    }

    const int mapPixelWidth = level.getWidth() * level.getTileSize();
    const int mapPixelHeight = level.getHeight() * level.getTileSize();
    const int maxCameraX = std::max(0, mapPixelWidth - getViewportWidth());
    const int maxCameraY = std::max(0, mapPixelHeight - windowHeight);

    cameraX = std::clamp(cameraX, 0, maxCameraX);
    cameraY = std::clamp(cameraY, 0, maxCameraY);
}


int MapEditorService::getViewportWidth() const {
    return std::max(1, windowWidth - kPaletteWidth);
}


int MapEditorService::getVisibleMapRows() const {
    return std::max(1, (windowHeight - kMapListTop - 12) / kMapListRowHeight);
}

bool MapEditorService::screenToCell(int screenX, int screenY,
                                    int& column, int& row) const {
    if (screenX < 0 || screenY < 0 ||
        screenX >= getViewportWidth() || screenY >= windowHeight) {
        return false;
    }

    const int tileSize = level.getTileSize();
    column = (screenX + cameraX) / tileSize;
    row = (screenY + cameraY) / tileSize;
    return level.isInside(column, row);
}

bool MapEditorService::paletteTileAt(int screenX, int screenY,
                                     TileId& tileId) const {
    const int localX = screenX - getViewportWidth() - kPalettePadding;
    const int localY = screenY - kPaletteTop;
    if (localX < 0 || localY < 0) {
        return false;
    }

    const int column = localX / kPaletteCellWidth;
    const int row = localY / kPaletteCellHeight;
    if (column < 0 || column >= kPaletteColumns) {
        return false;
    }

    const std::size_t index = static_cast<std::size_t>(
        row * kPaletteColumns + column);
    const TileDefinition* definition = paletteDefinition(index);
    if (definition == nullptr) {
        return false;
    }

    tileId = definition->tileId;
    return true;
}

bool MapEditorService::handlePanelClick(int screenX, int screenY) {
    const int panelX = getViewportWidth();
    if (screenX < panelX || screenX >= windowWidth) {
        return false;
    }

    const SDL_Rect tilesTab{
        panelX + kPalettePadding, kTabTop, kTabWidth, kTabHeight};
    const SDL_Rect mapsTab{
        panelX + kPalettePadding + kTabWidth + 8,
        kTabTop, kTabWidth, kTabHeight};
    if (contains(tilesTab, screenX, screenY)) {
        palettePage = PalettePage::Tiles;
        return true;
    }
    if (contains(mapsTab, screenX, screenY)) {
        palettePage = PalettePage::Maps;
        refreshSavedMaps();
        return true;
    }

    if (palettePage == PalettePage::Tiles) {
        return false;
    }

    const int buttonX = panelX + kPalettePadding;
    constexpr int smallButtonWidth = 40;
    constexpr int smallButtonGap = 4;
    const SDL_Rect widthMinus{buttonX, kMapButtonsTop, smallButtonWidth, 30};
    const SDL_Rect widthPlus{
        buttonX + smallButtonWidth + smallButtonGap,
        kMapButtonsTop, smallButtonWidth, 30};
    const SDL_Rect heightMinus{
        buttonX + (smallButtonWidth + smallButtonGap) * 2,
        kMapButtonsTop, smallButtonWidth, 30};
    const SDL_Rect heightPlus{
        buttonX + (smallButtonWidth + smallButtonGap) * 3,
        kMapButtonsTop, smallButtonWidth, 30};

    if (contains(widthMinus, screenX, screenY)) {
        resizeLevel(level.getWidth() - 1, level.getHeight());
    } else if (contains(widthPlus, screenX, screenY)) {
        resizeLevel(level.getWidth() + 1, level.getHeight());
    } else if (contains(heightMinus, screenX, screenY)) {
        resizeLevel(level.getWidth(), level.getHeight() - 1);
    } else if (contains(heightPlus, screenX, screenY)) {
        resizeLevel(level.getWidth(), level.getHeight() + 1);
    } else {
        const SDL_Rect newButton{buttonX, kMapActionTop, 84, 30};
        const SDL_Rect saveButton{buttonX + 92, kMapActionTop, 84, 30};
        const SDL_Rect saveAsButton{buttonX, kMapSaveAsTop, 176, 30};
        if (contains(newButton, screenX, screenY)) {
            createNewLevel();
        } else if (contains(saveButton, screenX, screenY)) {
            saveLevel();
        } else if (contains(saveAsButton, screenX, screenY)) {
            beginSaveAs();
        } else if (screenY >= kMapListTop) {
            const int visibleRow =
                (screenY - kMapListTop) / kMapListRowHeight;
            if (visibleRow < getVisibleMapRows()) {
                openSavedMap(static_cast<std::size_t>(
                    mapListOffset + visibleRow));
            }
        }
    }
    return true;
}

void MapEditorService::beginStroke() {
    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row)) {
        return;
    }

    // Bắt đầu trên đúng loại tile thì cả nét chuyển sang xóa.
    strokeTile = level.getTile(column, row) == selectedTile
                     ? kEmptyTileId
                     : selectedTile;
    strokeActive = true;
    paintStroke();
}

void MapEditorService::paintStroke() {
    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row) ||
        level.getTile(column, row) == strokeTile) {
        return;
    }

    level.setTile(column, row, strokeTile);
    dirty = true;
    pendingDiscardTarget.clear();
}

void MapEditorService::pickTile() {
    int column = 0;
    int row = 0;
    if (screenToCell(mouseX, mouseY, column, row)) {
        selectedTile = level.getTile(column, row);
    }
}

void MapEditorService::selectBrush(int paletteNumber) {
    if (paletteNumber <= 0) {
        return;
    }

    const TileDefinition* definition = paletteDefinition(
        static_cast<std::size_t>(paletteNumber - 1));
    if (definition != nullptr) {
        selectedTile = definition->tileId;
    }
}

void MapEditorService::generateLevel() {
    strokeActive = false;
    pendingDiscardTarget.clear();

    for (int row = 0; row < level.getHeight(); ++row) {
        for (int column = 0; column < level.getWidth(); ++column) {
            const TileId tileId = row == level.getHeight() - 1
                                      ? kStandardBrickTileId
                                      : kEmptyTileId;
            level.setTile(column, row, tileId);
        }
    }
    dirty = true;
}

void MapEditorService::createNewLevel() {
    if (!canDiscardChanges("NEW")) {
        return;
    }

    level = LevelData(level.getWidth(), level.getHeight(), level.getTileSize());
    mapPath.clear();
    cameraX = 0;
    cameraY = 0;
    generateLevel();
    statusMessage = "NEW MAP";
}

void MapEditorService::resizeLevel(int newWidth, int newHeight) {
    newWidth = std::clamp(newWidth, kMinMapDimension, kMaxMapDimension);
    newHeight = std::clamp(newHeight, kMinMapDimension, kMaxMapDimension);
    if (newWidth == level.getWidth() && newHeight == level.getHeight()) {
        return;
    }

    level.resize(newWidth, newHeight);
    dirty = true;
    pendingDiscardTarget.clear();
    updateCamera();
    statusMessage = "MAP RESIZED";
}

void MapEditorService::refreshSavedMaps() {
    savedMaps.clear();
    std::error_code error;
    const std::filesystem::path directory(mapDirectory);

    for (std::filesystem::directory_iterator it(directory, error), end;
         !error && it != end; it.increment(error)) {
        if (!it->is_regular_file(error)) {
            continue;
        }

        std::string extension = it->path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char character) {
                           return static_cast<char>(std::tolower(character));
                       });
        if (extension == ".map") {
            savedMaps.push_back(it->path().string());
        }
    }

    std::sort(savedMaps.begin(), savedMaps.end(),
              [](const std::string& left, const std::string& right) {
                  return mapName(left) < mapName(right);
              });
    mapListOffset = std::clamp(
        mapListOffset, 0,
        std::max(0, static_cast<int>(savedMaps.size()) - getVisibleMapRows()));
}

void MapEditorService::openSavedMap(std::size_t index) {
    if (index >= savedMaps.size()) {
        return;
    }

    const std::string target = savedMaps[index];
    if (!canDiscardChanges(target)) {
        return;
    }

    try {
        level = LevelCodec::load(target, level.getTileSize());
        mapPath = target;
        cameraX = 0;
        cameraY = 0;
        dirty = false;
        pendingDiscardTarget.clear();
        statusMessage = "MAP OPENED";
        std::cout << "Loaded map: " << mapPath << '\n';
    } catch (const std::exception& error) {
        statusMessage = "OPEN FAILED";
        std::cerr << "Cannot open map: " << error.what() << '\n';
    }
}

void MapEditorService::beginSaveAs() {
    saveAsName.clear();
    namingMap = true;
    palettePage = PalettePage::Maps;
    statusMessage = "TYPE MAP NAME";
    SDL_StartTextInput();
}

bool MapEditorService::handleSaveAsEvent(const SDL_Event& event) {
    if (event.type == SDL_TEXTINPUT) {
        for (const char character : std::string(event.text.text)) {
            const unsigned char value = static_cast<unsigned char>(character);
            if (saveAsName.size() < 32 &&
                (std::isalnum(value) || character == '_' || character == '-')) {
                saveAsName.push_back(character);
            }
        }
        return true;
    }

    if (event.type == SDL_KEYUP) {
        return false;
    }

    if (event.type != SDL_KEYDOWN || event.key.repeat != 0) {
        return true;
    }

    if (event.key.keysym.sym == SDLK_BACKSPACE && !saveAsName.empty()) {
        saveAsName.pop_back();
    } else if (event.key.keysym.sym == SDLK_RETURN) {
        saveLevelAs();
    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
        namingMap = false;
        statusMessage = "SAVE AS CANCELED";
        SDL_StopTextInput();
    }
    return true;
}

void MapEditorService::saveLevelAs() {
    if (saveAsName.empty()) {
        statusMessage = "NAME REQUIRED";
        return;
    }

    const std::filesystem::path target =
        std::filesystem::path(mapDirectory) / (saveAsName + ".map");
    const std::filesystem::path current(mapPath);
    // Save As chỉ được ghi đè khi đích chính là file đang mở.
    if (std::filesystem::exists(target) &&
        (mapPath.empty() || target.lexically_normal() != current.lexically_normal())) {
        statusMessage = "NAME EXISTS";
        return;
    }

    try {
        std::filesystem::create_directories(target.parent_path());
        LevelCodec::save(level, target.string());
        mapPath = target.string();
        dirty = false;
        namingMap = false;
        pendingDiscardTarget.clear();
        statusMessage = "MAP SAVED";
        SDL_StopTextInput();
        refreshSavedMaps();
        std::cout << "Saved map: " << mapPath << '\n';
    } catch (const std::exception& error) {
        statusMessage = "SAVE FAILED";
        std::cerr << "Cannot save map: " << error.what() << '\n';
    }
}

bool MapEditorService::canDiscardChanges(const std::string& target) {
    if (!dirty) {
        pendingDiscardTarget.clear();
        return true;
    }
    if (pendingDiscardTarget == target) {
        pendingDiscardTarget.clear();
        return true;
    }

    // Gửi lại đúng thao tác lần hai mới được bỏ phần chưa lưu.
    pendingDiscardTarget = target;
    statusMessage = "REPEAT TO DISCARD";
    return false;
}

void MapEditorService::saveLevel() {
    if (mapPath.empty()) {
        beginSaveAs();
        return;
    }

    try {
        LevelCodec::save(level, mapPath);
        dirty = false;
        pendingDiscardTarget.clear();
        statusMessage = "MAP SAVED";
        refreshSavedMaps();
        std::cout << "Saved map: " << mapPath << '\n';
    } catch (const std::exception& error) {
        statusMessage = "SAVE FAILED";
        std::cerr << "Cannot save map: " << error.what() << '\n';
    }
}

void MapEditorService::renderGrid(SDL_Renderer* renderer,
                                  SDL_Texture* worldTiles) {
    const int tileSize = level.getTileSize();
    const int firstColumn = cameraX / tileSize;
    const int firstRow = cameraY / tileSize;
    const int lastColumn = std::min(
        level.getWidth() - 1,
        (cameraX + getViewportWidth()) / tileSize);
    const int lastRow = std::min(
        level.getHeight() - 1,
        (cameraY + windowHeight) / tileSize);

    for (int row = firstRow; row <= lastRow; ++row) {
        for (int column = firstColumn; column <= lastColumn; ++column) {
            const SDL_Rect cell{
                column * tileSize - cameraX,
                row * tileSize - cameraY,
                tileSize,
                tileSize
            };

            renderTile(renderer, worldTiles, level.getTile(column, row), cell);
            setDrawColor(renderer, kGridColor);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}

void MapEditorService::renderBrushCursor(SDL_Renderer* renderer,
                                         SDL_Texture* worldTiles) {
    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row)) {
        return;
    }

    const int tileSize = level.getTileSize();
    const SDL_Rect cursor{
        column * tileSize - cameraX,
        row * tileSize - cameraY,
        tileSize,
        tileSize
    };
    const TileId previewTile = strokeActive ? strokeTile : selectedTile;

    if (!renderTile(renderer, worldTiles, previewTile, cursor, 155)) {
        setDrawColor(renderer, {210, 60, 60, 120});
        SDL_RenderFillRect(renderer, &cursor);
    }

    setDrawColor(renderer, kTextColor);
    SDL_RenderDrawRect(renderer, &cursor);
}

void MapEditorService::renderPalette(SDL_Renderer* renderer,
                                     SDL_Texture* worldTiles) {
    const int panelX = getViewportWidth();
    UiRenderer::fillRect(
        renderer,
        {panelX, 0, kPaletteWidth, windowHeight},
        kPanelColor);

    setDrawColor(renderer, {12, 14, 18, 255});
    SDL_RenderDrawLine(renderer, panelX, 0, panelX, windowHeight);

    UiRenderer::drawText(
        renderer, "EDITOR", panelX + kPalettePadding, 16, 3, kTextColor);

    const SDL_Rect tilesTab{
        panelX + kPalettePadding, kTabTop, kTabWidth, kTabHeight};
    const SDL_Rect mapsTab{
        panelX + kPalettePadding + kTabWidth + 8,
        kTabTop, kTabWidth, kTabHeight};
    renderButton(
        renderer, tilesTab, "TILES", palettePage == PalettePage::Tiles);
    renderButton(
        renderer, mapsTab, "MAPS", palettePage == PalettePage::Maps);

    const std::string status = statusMessage.empty()
                                   ? (dirty ? "UNSAVED" : "SAVED")
                                   : statusMessage;
    UiRenderer::drawText(
        renderer,
        fitLabel(status, 28),
        panelX + kPalettePadding,
        82,
        1,
        dirty ? kSelectedColor : kMutedTextColor);

    if (palettePage == PalettePage::Maps) {
        renderMapPalette(renderer);
        return;
    }

    std::size_t index = 0;
    while (const TileDefinition* definition = paletteDefinition(index)) {
        const int column = static_cast<int>(index) % kPaletteColumns;
        const int row = static_cast<int>(index) / kPaletteColumns;
        const SDL_Rect card{
            panelX + kPalettePadding + column * kPaletteCellWidth,
            kPaletteTop + row * kPaletteCellHeight,
            kPaletteCellWidth - 8,
            kPaletteCellHeight - 8
        };
        UiRenderer::fillRect(renderer, card, kCardColor);

        const SDL_Rect preview{
            card.x + (card.w - kPalettePreviewSize) / 2,
            card.y + 5,
            kPalettePreviewSize,
            kPalettePreviewSize
        };
        renderTile(renderer, worldTiles, definition->tileId, preview);
        UiRenderer::drawText(
            renderer,
            definition->label,
            card.x + 7,
            card.y + 61,
            1,
            kTextColor);

        if (definition->tileId == selectedTile) {
            setDrawColor(renderer, kSelectedColor);
            SDL_RenderDrawRect(renderer, &card);
            const SDL_Rect inner{card.x + 1, card.y + 1, card.w - 2, card.h - 2};
            SDL_RenderDrawRect(renderer, &inner);
        }
        ++index;
    }

    const TileDefinition* selected = findTileDefinition(selectedTile);
    UiRenderer::drawText(
        renderer, "SELECTED", panelX + kPalettePadding, 382, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer,
        selected != nullptr ? selected->label : "UNKNOWN",
        panelX + kPalettePadding,
        400,
        2,
        kSelectedColor);

    UiRenderer::drawText(
        renderer, "ZERO CLOSE", panelX + kPalettePadding, 468, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer, "CTRL S SAVE", panelX + kPalettePadding, 486, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer, "E PICK", panelX + kPalettePadding, 504, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer, "R RESET", panelX + kPalettePadding, 522, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer, "TAB MAPS", panelX + kPalettePadding, 540, 1,
        kMutedTextColor);
}

void MapEditorService::renderMapPalette(SDL_Renderer* renderer) {
    const int panelX = getViewportWidth();
    const int buttonX = panelX + kPalettePadding;
    const std::string currentName = namingMap
                                        ? saveAsName + "_"
                                        : (mapPath.empty()
                                               ? "UNTITLED"
                                               : mapName(mapPath));

    UiRenderer::drawText(
        renderer, namingMap ? "SAVE AS" : "CURRENT MAP",
        buttonX, 108, 1, kMutedTextColor);
    UiRenderer::drawText(
        renderer,
        namingMap ? fitTailLabel(currentName, 14)
                  : fitLabel(currentName, 14),
        buttonX, 126, 2, kSelectedColor);

    const std::string sizeText =
        "SIZE " + std::to_string(level.getWidth()) + "X" +
        std::to_string(level.getHeight());
    UiRenderer::drawText(
        renderer, sizeText, buttonX, 156, 1, kTextColor);

    constexpr int smallButtonWidth = 40;
    constexpr int smallButtonGap = 4;
    renderButton(
        renderer,
        {buttonX, kMapButtonsTop, smallButtonWidth, 30}, "W-");
    renderButton(
        renderer,
        {buttonX + smallButtonWidth + smallButtonGap,
         kMapButtonsTop, smallButtonWidth, 30}, "W+");
    renderButton(
        renderer,
        {buttonX + (smallButtonWidth + smallButtonGap) * 2,
         kMapButtonsTop, smallButtonWidth, 30}, "H-");
    renderButton(
        renderer,
        {buttonX + (smallButtonWidth + smallButtonGap) * 3,
         kMapButtonsTop, smallButtonWidth, 30}, "H+");

    renderButton(renderer, {buttonX, kMapActionTop, 84, 30}, "NEW");
    renderButton(renderer, {buttonX + 92, kMapActionTop, 84, 30}, "SAVE");
    renderButton(renderer, {buttonX, kMapSaveAsTop, 176, 30}, "SAVE AS");

    UiRenderer::drawText(
        renderer, "SAVED MAPS", buttonX, 304, 1, kMutedTextColor);

    const int visibleRows = getVisibleMapRows();
    for (int row = 0; row < visibleRows; ++row) {
        const std::size_t index = static_cast<std::size_t>(mapListOffset + row);
        if (index >= savedMaps.size()) {
            break;
        }

        const SDL_Rect item{
            buttonX,
            kMapListTop + row * kMapListRowHeight,
            176,
            kMapListRowHeight - 4
        };
        const bool current = !mapPath.empty() &&
            std::filesystem::path(savedMaps[index]).lexically_normal() ==
                std::filesystem::path(mapPath).lexically_normal();
        UiRenderer::fillRect(
            renderer, item, current ? kSelectedColor : kCardColor);
        UiRenderer::drawText(
            renderer, fitLabel(mapName(savedMaps[index]), 26),
            item.x + 6, item.y + 8, 1,
            current ? kPanelColor : kTextColor);
    }

    if (savedMaps.empty()) {
        UiRenderer::drawText(
            renderer, "NO MAPS", buttonX, kMapListTop + 8, 1,
            kMutedTextColor);
    }
}
