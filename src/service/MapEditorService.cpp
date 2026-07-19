#include "service/MapEditorService.h"

#include "service/LevelCodec.h"
#include "view/TileCatalog.h"
#include "view/UiRenderer.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <utility>

namespace {
constexpr int kCameraSpeed = 12;
constexpr int kPaletteWidth = 208;
constexpr int kPalettePadding = 12;
constexpr int kPaletteTop = 88;
constexpr int kPaletteColumns = 2;
constexpr int kPaletteCellWidth = 92;
constexpr int kPaletteCellHeight = 88;
constexpr int kPalettePreviewSize = 48;

constexpr SDL_Color kMapColor{100, 149, 237, 255};
constexpr SDL_Color kPanelColor{28, 32, 40, 255};
constexpr SDL_Color kCardColor{47, 54, 66, 255};
constexpr SDL_Color kGridColor{65, 86, 112, 150};
constexpr SDL_Color kTextColor{238, 242, 248, 255};
constexpr SDL_Color kMutedTextColor{166, 177, 194, 255};
constexpr SDL_Color kSelectedColor{255, 181, 46, 255};

/**
 * Đặt màu vẽ hiện tại cho SDL renderer.
 *
 * @param renderer Renderer cần thay đổi màu.
 * @param color Màu RGBA mới.
 */
void setDrawColor(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * Lấy tile thứ index đang được phép hiển thị trong palette.
 *
 * @param index Vị trí tile trong palette, không tính tile bị ẩn.
 * @return Định nghĩa tile tương ứng, hoặc nullptr nếu index vượt giới hạn.
 */
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

/**
 * Vẽ một tile từ spritesheet vào vùng đích.
 *
 * @param renderer Renderer nhận lệnh vẽ.
 * @param texture Spritesheet WorldTiles.png.
 * @param tileId Mã tile cần vẽ.
 * @param destination Vùng đích trên cửa sổ.
 * @param alpha Độ trong suốt tạm thời của texture.
 * @return true nếu tile có sprite hợp lệ; ngược lại là false.
 */
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
    SDL_SetTextureAlphaMod(texture, oldAlpha);
    return true;
}
}

/**
 * Khởi tạo editor nhúng và nạp map dùng chung với game.
 *
 * Nếu file chưa tồn tại hoặc không hợp lệ, editor tạo một map mới có hàng gạch
 * nền và đánh dấu dữ liệu chưa được lưu.
 *
 * @param mapWidth Số cột dùng khi cần tạo map mới.
 * @param mapHeight Số hàng dùng khi cần tạo map mới.
 * @param tileSize Kích thước một ô tile theo pixel.
 * @param windowWidth Chiều rộng cửa sổ game.
 * @param windowHeight Chiều cao cửa sổ game.
 * @param mapPath Đường dẫn file map dùng chung.
 */
MapEditorService::MapEditorService(int mapWidth, int mapHeight, int tileSize,
                                   int windowWidth, int windowHeight,
                                   std::string mapPath)
    : level(mapWidth, mapHeight, tileSize),
      mapPath(std::move(mapPath)),
      windowWidth(windowWidth),
      windowHeight(windowHeight),
      cameraX(0),
      cameraY(0),
      mouseX(0),
      mouseY(0),
      selectedTile(kStandardBrickTileId),
      strokeTile(kEmptyTileId),
      editorEnabled(false),
      dirty(false),
      strokeActive(false) {
    try {
        level = LevelCodec::load(this->mapPath, tileSize);
        std::cout << "Loaded map: " << this->mapPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\nCreating a new map instead.\n";
        generateLevel();
    }
}

/**
 * @return true nếu editor đang nhận input và hiển thị palette.
 */
bool MapEditorService::isEnabled() const {
    return editorEnabled;
}

/**
 * @return true nếu level đã thay đổi kể từ lần lưu gần nhất.
 */
bool MapEditorService::isDirty() const {
    return dirty;
}

/**
 * Xử lý input editor trong SDL event loop do Game sở hữu.
 *
 * Phím 0 bật/tắt editor. Khi editor mở, phím 1 đến 5 chọn palette, E lấy tile,
 * R tạo lại map, Ctrl+S lưu và Esc đóng editor. Chuột trái chọn palette hoặc
 * kéo một nét đặt/xóa tile trong viewport map.
 *
 * @param event Sự kiện SDL cần xử lý.
 * @return true nếu editor đã sử dụng event; ngược lại là false.
 */
bool MapEditorService::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
        event.key.keysym.sym == SDLK_0) {
        editorEnabled = !editorEnabled;
        strokeActive = false;
        return true;
    }

    if (!editorEnabled) {
        return false;
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

        TileId paletteTile = kEmptyTileId;
        if (paletteTileAt(mouseX, mouseY, paletteTile)) {
            selectedTile = paletteTile;
            strokeActive = false;
        } else {
            beginStroke();
        }
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
    } else if (key >= SDLK_1 && key <= SDLK_5) {
        selectBrush(static_cast<int>(key - SDLK_0));
    } else if (key == SDLK_e) {
        pickTile();
    } else if (key == SDLK_r) {
        generateLevel();
    } else if (key == SDLK_s &&
               (event.key.keysym.mod & KMOD_CTRL) != 0) {
        saveLevel();
    }
    return true;
}

/**
 * Cập nhật camera editor theo trạng thái bàn phím hiện tại.
 */
void MapEditorService::update() {
    if (editorEnabled) {
        updateCamera();
        if (strokeActive) {
            paintStroke();
        }
    }
}

/**
 * Vẽ map, con trỏ brush và palette vào renderer của Game.
 *
 * @param renderer Renderer thuộc sở hữu Game.
 * @param worldTiles Spritesheet tile đã được Game nạp một lần.
 */
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

/**
 * @return LevelData hiện đang được editor quản lý.
 */
const LevelData& MapEditorService::getLevel() const {
    return level;
}

/**
 * @return Tọa độ x hiện tại của camera editor.
 */
int MapEditorService::getCameraX() const {
    return cameraX;
}

/**
 * @return Tọa độ y hiện tại của camera editor.
 */
int MapEditorService::getCameraY() const {
    return cameraY;
}

/**
 * @return Viewport map nằm bên trái palette.
 */
SDL_Rect MapEditorService::getMapViewport() const {
    return {0, 0, getViewportWidth(), windowHeight};
}

/**
 * Cập nhật camera từ WASD hoặc các phím mũi tên và giữ camera trong map.
 */
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

/**
 * @return Chiều rộng phần cửa sổ dành cho map.
 */
int MapEditorService::getViewportWidth() const {
    return std::max(1, windowWidth - kPaletteWidth);
}

/**
 * Chuyển tọa độ chuột trong viewport sang ô map có tính camera.
 *
 * @param screenX Tọa độ x chuột trong cửa sổ.
 * @param screenY Tọa độ y chuột trong cửa sổ.
 * @param column Biến nhận chỉ số cột.
 * @param row Biến nhận chỉ số hàng.
 * @return true nếu tọa độ thuộc một ô hợp lệ; ngược lại là false.
 */
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

/**
 * Tìm tile palette nằm dưới con trỏ chuột.
 *
 * @param screenX Tọa độ x chuột trong cửa sổ.
 * @param screenY Tọa độ y chuột trong cửa sổ.
 * @param tileId Biến nhận mã tile khi tìm thấy.
 * @return true nếu chuột nằm trên một ô palette; ngược lại là false.
 */
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

/**
 * Bắt đầu nét đặt hoặc xóa dựa trên nội dung ô đầu tiên.
 */
void MapEditorService::beginStroke() {
    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row)) {
        return;
    }

    strokeTile = level.getTile(column, row) == selectedTile
                     ? kEmptyTileId
                     : selectedTile;
    strokeActive = true;
    paintStroke();
}

/**
 * Gán tile của nét hiện tại vào ô dưới con trỏ chuột.
 */
void MapEditorService::paintStroke() {
    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row) ||
        level.getTile(column, row) == strokeTile) {
        return;
    }

    level.setTile(column, row, strokeTile);
    dirty = true;
}

/**
 * Chọn tile dưới con trỏ làm brush hiện tại.
 */
void MapEditorService::pickTile() {
    int column = 0;
    int row = 0;
    if (screenToCell(mouseX, mouseY, column, row)) {
        selectedTile = level.getTile(column, row);
    }
}

/**
 * Chọn trực tiếp một brush theo thứ tự hiển thị trong palette.
 *
 * @param paletteNumber Số thứ tự bắt đầu từ 1.
 */
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

/**
 * Tạo map rỗng với một hàng StandardBrick làm mặt đất.
 */
void MapEditorService::generateLevel() {
    strokeActive = false;

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

/**
 * Ghi LevelData hiện tại về file map dùng chung với game.
 */
void MapEditorService::saveLevel() {
    try {
        LevelCodec::save(level, mapPath);
        dirty = false;
        std::cout << "Saved map: " << mapPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Cannot save map: " << error.what() << '\n';
    }
}

/**
 * Vẽ các tile nhìn thấy trong viewport camera.
 *
 * @param renderer Renderer nhận lệnh vẽ.
 * @param worldTiles Spritesheet chứa tile.
 */
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

/**
 * Vẽ preview brush trong suốt tại ô dưới con trỏ chuột.
 *
 * @param renderer Renderer nhận lệnh vẽ.
 * @param worldTiles Spritesheet chứa tile.
 */
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

/**
 * Vẽ panel palette cố định ở cạnh phải cửa sổ.
 *
 * @param renderer Renderer nhận lệnh vẽ.
 * @param worldTiles Spritesheet chứa tile preview.
 */
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
    UiRenderer::drawText(
        renderer,
        dirty ? "UNSAVED" : "SAVED",
        panelX + kPalettePadding,
        52,
        1,
        dirty ? kSelectedColor : kMutedTextColor);

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
        renderer, "SELECTED", panelX + kPalettePadding, 372, 1,
        kMutedTextColor);
    UiRenderer::drawText(
        renderer,
        selected != nullptr ? selected->label : "UNKNOWN",
        panelX + kPalettePadding,
        390,
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
}
