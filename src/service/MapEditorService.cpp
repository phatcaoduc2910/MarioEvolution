#include "service/MapEditorService.h"

#include "service/LevelCodec.h"

#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <utility>

namespace {
constexpr int kCameraSpeed = 12;
constexpr std::array<TileId, 5> kBrushTiles{
    kStandardBrickTileId,
    kCoinBrickTileId,
    kMushroomBrickTileId,
    kFlowerBrickTileId,
    kCoinTileId
};

/**
 * Đặt màu vẽ hiện tại cho SDL renderer.
 *
 * @param renderer Renderer cần thay đổi màu.
 * @param color Màu RGBA mới.
 */
void setDrawColor(SDL_Renderer* renderer, const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * Lấy màu đại diện cho một mã tile trong editor.
 *
 * @param tileId Mã tile cần hiển thị.
 * @return Màu dùng để vẽ tile.
 */
SDL_Color tileColor(TileId tileId) {
    switch (tileId) {
        case kEmptyTileId: return {24, 25, 28, 255};
        case kStandardBrickTileId: return {139, 90, 43, 255};
        case kCoinBrickTileId: return {235, 178, 45, 255};
        case kMushroomBrickTileId: return {196, 75, 52, 255};
        case kFlowerBrickTileId: return {50, 160, 90, 255};
        case kCoinTileId: return {255, 215, 0, 255};
        default: return {190, 190, 190, 255};
    }
}
}

/**
 * Khởi tạo editor cùng một LevelData rỗng và vùng nhìn cố định.
 *
 * @param mapWidth Số cột tile của map.
 * @param mapHeight Số hàng tile của map.
 * @param tileSize Kích thước cạnh của một tile, tính bằng pixel.
 * @param windowWidth Chiều rộng cửa sổ editor.
 * @param windowHeight Chiều cao cửa sổ editor.
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
      editorEnabled(true),
      strokeActive(false),
      running(false),
      window(nullptr),
      renderer(nullptr) {
    try {
        level = LevelCodec::load(this->mapPath, tileSize);
        std::cout << "Loaded map: " << this->mapPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\nCreating a new map instead.\n";
        generateLevel();
    }
}

/**
 * Giải phóng SDL renderer và cửa sổ thuộc sở hữu của editor.
 */
MapEditorService::~MapEditorService() {
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
}

/**
 * Tạo cửa sổ và renderer cần thiết cho editor.
 *
 * Renderer phần mềm được dùng làm phương án dự phòng khi renderer tăng tốc
 * không khả dụng.
 *
 * @return true nếu editor khởi tạo thành công; ngược lại là false.
 */
bool MapEditorService::start() {
    window = SDL_CreateWindow(
        "MarioEvolution Tile Editor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Không thể tạo cửa sổ map editor: "
                  << SDL_GetError() << '\n';
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderer == nullptr) {
        std::cerr << "Không thể tạo renderer cho map editor: "
                  << SDL_GetError() << '\n';
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

/**
 * Chạy vòng lặp input, camera và render của editor.
 */
void MapEditorService::run() {
    if (renderer == nullptr) {
        return;
    }

    running = true;
    while (running) {
        const Uint32 frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            handleEvent(event);
        }

        updateCamera();
        if (strokeActive) {
            paintStroke();
        }
        render();

        const Uint32 frameTime = SDL_GetTicks() - frameStart;
        constexpr Uint32 kMinimumFrameTime = 16;
        if (frameTime < kMinimumFrameTime) {
            SDL_Delay(kMinimumFrameTime - frameTime);
        }
    }
}

/**
 * Xử lý phím điều khiển editor và nét vẽ bằng chuột.
 *
 * Phím 0 bật hoặc tắt editor, phím 1 đến 5 chọn brush, E lấy tile dưới
 * con trỏ, R tạo lại level và S lưu map.
 *
 * @param event Sự kiện SDL cần xử lý.
 */
void MapEditorService::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        running = false;
        return;
    }

    if (event.type == SDL_MOUSEMOTION) {
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        return;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
        mouseX = event.button.x;
        mouseY = event.button.y;
        beginStroke();
        return;
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
        strokeActive = false;
        return;
    }

    if (event.type != SDL_KEYDOWN || event.key.repeat != 0) {
        return;
    }

    const SDL_Keycode key = event.key.keysym.sym;
    if (key == SDLK_ESCAPE) {
        running = false;
    } else if (key == SDLK_0) {
        editorEnabled = !editorEnabled;
        strokeActive = false;
    } else if (key >= SDLK_1 && key <= SDLK_5) {
        selectBrush(static_cast<int>(key - SDLK_0));
    } else if (key == SDLK_e && editorEnabled) {
        pickTile();
    } else if (key == SDLK_r && editorEnabled) {
        generateLevel();
    } else if (key == SDLK_s && editorEnabled) {
        saveLevel();
    }
}

/**
 * Cập nhật camera từ các phím WASD hoặc phím mũi tên đang được giữ.
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
    const int maxCameraX = std::max(0, mapPixelWidth - windowWidth);
    const int maxCameraY = std::max(0, mapPixelHeight - windowHeight);

    cameraX = std::clamp(cameraX, 0, maxCameraX);
    cameraY = std::clamp(cameraY, 0, maxCameraY);
}

/**
 * Chuyển tọa độ chuột trên màn hình sang ô trong map có tính camera.
 *
 * @param screenX Tọa độ x của chuột trong cửa sổ.
 * @param screenY Tọa độ y của chuột trong cửa sổ.
 * @param column Biến nhận chỉ số cột tile.
 * @param row Biến nhận chỉ số hàng tile.
 * @return true nếu ô nằm trong map; ngược lại là false.
 */
bool MapEditorService::screenToCell(int screenX, int screenY,
                                    int& column, int& row) const {
    if (screenX < 0 || screenY < 0 ||
        screenX >= windowWidth || screenY >= windowHeight) {
        return false;
    }

    const int tileSize = level.getTileSize();
    column = (screenX + cameraX) / tileSize;
    row = (screenY + cameraY) / tileSize;
    return level.isInside(column, row);
}

/**
 * Bắt đầu một nét vẽ và chọn thao tác đặt hoặc xóa từ ô đầu tiên.
 *
 * Nếu ô đầu tiên đã chứa selectedTile, toàn bộ nét kéo sẽ xóa. Nếu không,
 * toàn bộ nét kéo sẽ đặt selectedTile giống hành vi brush trong tutorial.
 */
void MapEditorService::beginStroke() {
    if (!editorEnabled) {
        return;
    }

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
 * Gán brush của nét hiện tại vào ô dưới con trỏ chuột.
 */
void MapEditorService::paintStroke() {
    int column = 0;
    int row = 0;
    if (screenToCell(mouseX, mouseY, column, row)) {
        level.setTile(column, row, strokeTile);
    }
}

/**
 * Chọn mã tile đang nằm dưới con trỏ làm brush hiện tại.
 */
void MapEditorService::pickTile() {
    int column = 0;
    int row = 0;
    if (screenToCell(mouseX, mouseY, column, row)) {
        selectedTile = level.getTile(column, row);
    }
}

/**
 * Chọn trực tiếp brush theo phím 1 đến 5.
 */
void MapEditorService::selectBrush(int key) {
    const std::size_t index = static_cast<std::size_t>(key - 1);
    if (index < kBrushTiles.size()) {
        selectedTile = kBrushTiles[index];
    }
}

/**
 * Tạo lại map rỗng với một hàng StandardBrick làm mặt đất.
 */
void MapEditorService::generateLevel() {
    strokeActive = false;

    for (int row = 0; row < level.getHeight(); ++row) {
        for (int column = 0; column < level.getWidth(); ++column) {
            TileId tileId = kEmptyTileId;

            if (row == level.getHeight() - 1) {
                tileId = kStandardBrickTileId;
            }

            level.setTile(column, row, tileId);
        }
    }
}

/**
 * Ghi trạng thái editor hiện tại về file map dùng chung với game.
 */
void MapEditorService::saveLevel() {
    try {
        LevelCodec::save(level, mapPath);
        std::cout << "Saved map: " << mapPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Cannot save map: " << error.what() << '\n';
    }
}

/**
 * Vẽ toàn bộ trạng thái hiện tại của editor.
 */
void MapEditorService::render() {
    setDrawColor(renderer, {18, 19, 22, 255});
    SDL_RenderClear(renderer);

    renderGrid();
    renderBrushCursor();

    SDL_RenderPresent(renderer);
}

/**
 * Chỉ vẽ các tile đang nằm trong vùng camera.
 */
void MapEditorService::renderGrid() {
    const int tileSize = level.getTileSize();
    const int firstColumn = cameraX / tileSize;
    const int firstRow = cameraY / tileSize;
    const int lastColumn = std::min(
        level.getWidth() - 1,
        (cameraX + windowWidth) / tileSize);
    const int lastRow = std::min(
        level.getHeight() - 1,
        (cameraY + windowHeight) / tileSize);

    for (int row = firstRow; row <= lastRow; ++row) {
        for (int column = firstColumn; column <= lastColumn; ++column) {
            const SDL_Rect cell = {
                column * tileSize - cameraX,
                row * tileSize - cameraY,
                tileSize,
                tileSize
            };

            setDrawColor(renderer, tileColor(level.getTile(column, row)));
            SDL_RenderFillRect(renderer, &cell);

            if (editorEnabled) {
                setDrawColor(renderer, {58, 61, 68, 255});
                SDL_RenderDrawRect(renderer, &cell);
            }
        }
    }
}

/**
 * Vẽ brush trong suốt tại ô đang nằm dưới con trỏ chuột.
 */
void MapEditorService::renderBrushCursor() {
    if (!editorEnabled) {
        return;
    }

    int column = 0;
    int row = 0;
    if (!screenToCell(mouseX, mouseY, column, row)) {
        return;
    }

    const int tileSize = level.getTileSize();
    const SDL_Rect cursor = {
        column * tileSize - cameraX,
        row * tileSize - cameraY,
        tileSize,
        tileSize
    };

    SDL_Color color = tileColor(strokeActive ? strokeTile : selectedTile);
    color.a = 150;
    setDrawColor(renderer, color);
    SDL_RenderFillRect(renderer, &cursor);

    setDrawColor(renderer, {245, 245, 245, 255});
    SDL_RenderDrawRect(renderer, &cursor);
}
