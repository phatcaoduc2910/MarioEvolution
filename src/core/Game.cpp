#include "core/Game.h"

#include "model/Boss.h"
#include "service/MapEditorService.h"
#include "view/UiRenderer.h"


#include <SDL2/SDL_image.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>

namespace {
constexpr const char* kLevelPath = "assets/maps/normal_1.map";
constexpr const char* kMapDirectory = "assets/maps";
constexpr int kMapWidth = 25;
constexpr int kMapHeight = 19;
constexpr int kTileSize = 32;
constexpr int kPauseButtonWidth = 280;
constexpr int kPauseButtonHeight = 48;
constexpr int kPauseButtonGap = 14;
constexpr SDL_Color kAudioOnColor{42, 132, 92, 235};
constexpr SDL_Color kAudioOffColor{166, 65, 65, 235};
constexpr SDL_Color kMenuButtonColor{69, 88, 120, 245};
constexpr SDL_Color kAudioTextColor{255, 255, 255, 255};
constexpr double kMenuBackgroundPixelsPerMs = 0.02;
constexpr double kGameplayBackgroundParallax = 0.08;

// Vẽ và xử lý các nút điều khiển trong menu tạm dừng.
struct PauseMenuLayout {
    SDL_Rect music;
    SDL_Rect sfx;
    SDL_Rect mainMenu;
};

// Tính vị trí các nút của menu tạm dừng.
bool pauseMenuLayout(SDL_Renderer* renderer, PauseMenuLayout& layout) {
    int screenWidth = 0;
    int screenHeight = 0;
    if (renderer == nullptr ||
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0 ||
        screenWidth <= 0 || screenHeight <= 0) {
        return false;
    }

    layout.music = {
        (screenWidth - kPauseButtonWidth) / 2,
        screenHeight / 2 - 85,
        kPauseButtonWidth,
        kPauseButtonHeight};
    layout.sfx = {
        layout.music.x,
        layout.music.y + kPauseButtonHeight + kPauseButtonGap,
        kPauseButtonWidth,
        kPauseButtonHeight};
    layout.mainMenu = {
        layout.music.x,
        layout.sfx.y + kPauseButtonHeight + kPauseButtonGap,
        kPauseButtonWidth,
        kPauseButtonHeight};
    return true;
}

// Vẽ nhãn cho một nút trong menu tạm dừng.
void drawPauseButtonLabel(SDL_Renderer* renderer, const SDL_Rect& button,
                          std::string_view label) {
    constexpr int kTextScale = 2;
    constexpr int kGlyphAdvance = 6;
    constexpr int kGlyphHeight = 7;
    const int textWidth =
        static_cast<int>(label.size()) * kGlyphAdvance * kTextScale;
    UiRenderer::drawText(
        renderer, label,
        button.x + (button.w - textWidth) / 2,
        button.y + (button.h - kGlyphHeight * kTextScale) / 2,
        kTextScale, kAudioTextColor);
}

// Vẽ các nút điều khiển âm thanh và quay lại menu.
void renderPauseMenuControls(SDL_Renderer* renderer,
                             const AudioService& audio) {
    PauseMenuLayout layout;
    if (!pauseMenuLayout(renderer, layout)) {
        return;
    }

    UiRenderer::fillRect(
        renderer, layout.music,
        audio.isMusicMuted() ? kAudioOffColor : kAudioOnColor);
    UiRenderer::fillRect(
        renderer, layout.sfx,
        audio.isSfxMuted() ? kAudioOffColor : kAudioOnColor);
    UiRenderer::fillRect(renderer, layout.mainMenu, kMenuButtonColor);
    drawPauseButtonLabel(
        renderer, layout.music,
        audio.isMusicMuted() ? "MUSIC: OFF" : "MUSIC: ON");
    drawPauseButtonLabel(
        renderer, layout.sfx,
        audio.isSfxMuted() ? "SFX: OFF" : "SFX: ON");
    drawPauseButtonLabel(renderer, layout.mainMenu, "MAIN MENU");
}
}

Game::Game()
    : audioService(std::make_unique<SoundManager>()) {}

Game::~Game() {
    mapEditor.reset();
    textureManager.reset();
    audioService.reset();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
//Khởi tạo giá trị ban đầu của game
bool Game::start() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    // Khởi tạo SDL_image và cấu hình renderer cho game.
    constexpr int imageFlags = IMG_INIT_PNG;
    if ((IMG_Init(imageFlags) & imageFlags) != imageFlags) {
        SDL_Log("SDL_image initialization failed: %s", IMG_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "MarioEvolution",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == nullptr) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return false;
    }

    textureManager = std::make_unique<TextureManager>(renderer);
    if (!textureManager->gameTextureLoad()) {
        SDL_Log("Failed to load game textures.");
        return false;
    }

    audioService->load("jump", "assets/audio/sfx/jump.wav");
    // Nạp âm thanh khi người chơi mất mạng hoặc nhặt item.
    audioService->load("lose_life", "assets/audio/sfx/oh_no.wav");
    audioService->load("item_pickup", "assets/audio/sfx/item-pick-up.wav");
    audioService->load("fireball", "assets/audio/sfx/fireball.wav");
    // Nạp âm thanh khi hoàn thành hoặc thất bại màn chơi.
    audioService->load("win", "assets/audio/sfx/goal.wav");
    audioService->load("gameover", "assets/audio/sfx/gameover.wav");
    // Nạp nhạc nền cho gameplay.
    audioService->load("theme", "assets/audio/music/theme.mp3");

    discoverLevels();
    if (levelPaths.empty()) {
        SDL_Log("No playable maps found in %s.", kMapDirectory);
        return false;
    }
    loadSelectedLevel();

    lastFrameTicks = SDL_GetTicks();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return true;
}

// Chuyển game sang trạng thái tạm dừng.
void Game::pause() {
    currentGameState = Paused;
    pauseScreen.render(renderer);
    audioService->pause("theme");
}

// Tiếp tục game và phát lại nhạc nền.
void Game::resume() {
    currentGameState = Playing;
    // Tiếp tục nhạc nền khi quay lại gameplay.
    audioService->play("theme", true);
}

void Game::edit() {
    currentGameState = Editing;
    audioService->pause("theme");
}

// Tìm và sắp xếp các màn chơi có trong thư mục map.
void Game::discoverLevels() {
    levelPaths.clear();
    std::error_code error;
    for (std::filesystem::directory_iterator it(kMapDirectory, error), end;
         !error && it != end; it.increment(error)) {
        if (it->is_regular_file(error) && it->path().extension() == ".map") {
            levelPaths.push_back(it->path().generic_string());
        }
    }
    std::sort(levelPaths.begin(), levelPaths.end());

    const auto defaultLevel = std::find(
        levelPaths.begin(), levelPaths.end(), std::string(kLevelPath));
    selectedLevelIndex = defaultLevel == levelPaths.end()
                             ? 0
                             : static_cast<std::size_t>(
                                   std::distance(levelPaths.begin(), defaultLevel));
}

// Nạp màn chơi đang được chọn vào thế giới game.
void Game::loadSelectedLevel() {
    if (levelPaths.empty()) return;
    mapEditor = std::make_unique<MapEditorService>(
        kMapWidth, kMapHeight, kTileSize, WINDOW_WIDTH, WINDOW_HEIGHT,
        levelPaths[selectedLevelIndex]);
    world = World();
    world.loadLevel(mapEditor->getLevel());
    bossArena.reset();
}

// Chuyển lựa chọn giữa các màn chơi.
void Game::selectLevel(int direction) {
    if (levelPaths.empty() || direction == 0) return;
    const auto count = static_cast<long long>(levelPaths.size());
    const auto current = static_cast<long long>(selectedLevelIndex);
    selectedLevelIndex = static_cast<std::size_t>(
        (current + direction + count) % count);
    loadSelectedLevel();
}

// Đồng bộ màn đang chọn sau khi chỉnh sửa map.
void Game::syncSelectedLevel(const std::string& mapPath) {
    const auto target = std::filesystem::path(mapPath).lexically_normal();
    discoverLevels();
    const auto selected = std::find_if(
        levelPaths.begin(), levelPaths.end(),
        [&target](const std::string& path) {
            return std::filesystem::path(path).lexically_normal() == target;
        });
    if (selected != levelPaths.end()) {
        selectedLevelIndex = static_cast<std::size_t>(
            std::distance(levelPaths.begin(), selected));
    }
}

std::string Game::selectedLevelName() const {
    if (levelPaths.empty()) return "NO MAP";
    return std::filesystem::path(levelPaths[selectedLevelIndex]).stem().string();
}

// Thực hiện hành động được chọn trong menu chính.
void Game::activateStartMenuAction(StartMenuAction action) {
    if (action == StartMenuAction::StartGame) {
        startLevel();
    } else if (action == StartMenuAction::MapEditor) {
        mapEditor->open();
        world.getPlayer().setMoveDirection(0);
        edit();
    } else if (action == StartMenuAction::ExitGame) {
        currentGameState = Exit;
    }
}

void Game::startLevel() {
    // Khởi tạo lại dữ liệu và trạng thái cho một lượt chơi mới.
    world = World();
    world.loadLevel(mapEditor->getLevel());
    // Retry phải dựng lại boss, hazard và timer từ đầu.
    bossArena.reset();
    camera.reset();
    gameplayBackgroundOffset = 0.0;
    currentGameState = Playing;
    audioService->play("theme", true);
}

// Xử lý vòng lặp game
void Game::gameLoop() {
    SDL_Event event;

    while (currentGameState != Exit) {
        while (SDL_PollEvent(&event)) {

            if (currentGameState == Paused &&
                event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {
                PauseMenuLayout layout;
                const SDL_Point click{event.button.x, event.button.y};
                const bool hasLayout = pauseMenuLayout(renderer, layout);
                if (hasLayout && SDL_PointInRect(&click, &layout.music)) {
                    const bool muted = !audioService->isMusicMuted();
                    audioService->setMusicMuted(muted);
                    continue;
                }
                if (hasLayout && SDL_PointInRect(&click, &layout.sfx)) {
                    audioService->setSfxMuted(!audioService->isSfxMuted());
                    continue;
                }
                if (hasLayout && SDL_PointInRect(&click, &layout.mainMenu)) {
                    currentGameState = StartMenu;
                    continue;
                }
            }

            Key key = Key::None;
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                key = inputHandler.mapKey(event.key.keysym.sym);
                inputHandler.press(key);
            } else if (event.type == SDL_KEYUP) {
                key = inputHandler.mapKey(event.key.keysym.sym);
                // Xóa phím khỏi trạng thái đang được giữ khi nhận KEYUP.
                inputHandler.release(key);
            }

            if(event.type == SDL_QUIT) {
                currentGameState = Exit;
            }
            switch (currentGameState) {
                case StartMenu:{
                    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                        if (key == Key::Up) startScreen.moveSelection(-1);
                        else if (key == Key::Down) startScreen.moveSelection(1);
                        else if (key == Key::Left) selectLevel(-1);
                        else if (key == Key::Right) selectLevel(1);
                        else if (key == Key::Enter) {
                            activateStartMenuAction(
                                startScreen.getSelectedAction());
                        }
                    } else if (event.type == SDL_MOUSEBUTTONDOWN &&
                               event.button.button == SDL_BUTTON_LEFT) {
                        const int levelDirection = startScreen.levelDirectionAt(
                            renderer, event.button.x, event.button.y);
                        if (levelDirection != 0) {
                            selectLevel(levelDirection);
                        } else {
                            activateStartMenuAction(startScreen.actionAt(
                                renderer, event.button.x, event.button.y));
                        }
                    }
                    break;
                }
                case Playing:{
                    if(inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
                        pause();
                    } else if (event.type == SDL_KEYDOWN &&
                               key == Key::Fire) {
                        if (world.shootFireball()) {
                            audioService->play("fireball");
                        }
                    } else if (inputHandler.isPressed(Key::Jump)) {
                        Player& player = world.getPlayer();
                        const bool canJump = player.isAlive() && player.isOnGround();
                        player.jump();
                        if (canJump) {
                            // Phát âm thanh khi người chơi thực sự có thể nhảy.
                            audioService->play("jump");
                        }
                    }
                    break;
                }
                case Paused:{
                    if(inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
                        resume();
                    }
                    break;
                }
                case Editing:{
                    //Vào map editor
                    const bool wasEditorEnabled = mapEditor->isEnabled();
                    if (mapEditor->handleEvent(event)) {
                        if (wasEditorEnabled && !mapEditor->isEnabled()) {
                            syncSelectedLevel(mapEditor->getMapPath());
                            world.loadLevel(mapEditor->getLevel());
                            currentGameState = StartMenu;
                        }
                        continue;
                    }
                    break;
                }
                case LevelComplete:
                case GameOver:{
                    if (inputHandler.isPressed(Key::Enter)) {
                        inputHandler.release(Key::Enter);
                        // Bắt đầu lại màn chơi khi nhấn Enter.
                        startLevel();
                    } else if (inputHandler.isPressed(Key::Esc)) {
                        inputHandler.release(Key::Esc);
                        currentGameState = StartMenu;
                    }
                    break;
                }
                case Exit:{
                    break;
                }
            }
        }

    constexpr int kFixedStepMs = 11;
    // Cập nhật gameplay theo fixed timestep.
    constexpr double kFixedStepSeconds = kFixedStepMs / 1000.0;
    static int accumulatorMs = 0;
    const Uint32 now = SDL_GetTicks();
    const Uint32 elapsed = now - lastFrameTicks;
    const int deltaMs = static_cast<int>(std::min<Uint32>(elapsed, 100));
    lastFrameTicks = now;
    // Chỉ tích lũy thời gian khi game đang chơi.
    if (currentGameState == Playing) {
        accumulatorMs += deltaMs;
    } else {
        accumulatorMs = 0;
    }
    // Cập nhật background chuyển động của menu chính.
    if (currentGameState == StartMenu) {
        menuBackgroundOffset += deltaMs * kMenuBackgroundPixelsPerMs;
    }


    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
    SDL_RenderClear(renderer);

    switch (currentGameState) {
        case StartMenu:
            // Hiển thị background menu và tên màn chơi đã chọn.
            worldRenderer.renderScrollingBackground(
                renderer, *textureManager, WINDOW_WIDTH, WINDOW_HEIGHT,
                static_cast<int>(menuBackgroundOffset));
            startScreen.render(renderer, selectedLevelName());
            break;

        case Paused:
            pauseScreen.render(renderer);
            renderPauseMenuControls(renderer, *audioService);
            break;
        case Playing:{
            while (accumulatorMs >= kFixedStepMs) {
                int horizontalInput = 0;
                if (inputHandler.isPressed(Key::Left)) --horizontalInput;
                if (inputHandler.isPressed(Key::Right)) ++horizontalInput;
                world.getPlayer().setMoveDirection(horizontalInput);
                const double playerXBeforeStep = world.getPlayer().getX();
                const int livesBeforeStep = world.getLives();
                world.update(kFixedStepSeconds, camera.getX());
                collisionSystem.update(world, kFixedStepSeconds);
                bossArena.update(world, kFixedStepSeconds);
                const bool collectedItem = std::any_of(
                    world.getItems().begin(), world.getItems().end(),
                    [](const std::unique_ptr<Item>& item) {
                        return item->isCollected();
                    });
                if (!world.isGameOver() &&
                    world.getLives() < livesBeforeStep) {
                    camera.resetTo(
                        static_cast<int>(world.getPlayer().getX()));
                    // Phát âm thanh khi người chơi mất mạng.
                    audioService->play("lose_life");
                } else {
                    // Cập nhật offset parallax theo chuyển động ngang của người chơi.
                    gameplayBackgroundOffset +=
                        (world.getPlayer().getX() - playerXBeforeStep) *
                        kGameplayBackgroundParallax;
                }
                if (collectedItem) {
                    // Phát âm thanh khi người chơi nhặt item.
                    audioService->play("item_pickup");
                }
                if (world.isLevelComplete()) {
                    // Chuyển trạng thái và phát âm thanh khi hoàn thành màn chơi.
                    currentGameState = LevelComplete;
                    audioService->pause("theme");
                    audioService->play("win");
                } else if (world.isGameOver()) {
                    // Chuyển trạng thái và phát âm thanh khi game over.
                    currentGameState = GameOver;
                    audioService->pause("theme");
                    audioService->play("gameover");
                }

                if (currentGameState != Playing) {
                    accumulatorMs = 0;
                    break;
                }
                accumulatorMs -= kFixedStepMs;
            }
            worldRenderer.update(deltaMs);
            actorRenderer.updatePlayer(world.getPlayer(), deltaMs);
            actorRenderer.updateWorld(world, deltaMs);

            const LevelData& level = mapEditor->getLevel();
            const int worldWidth = level.getWidth() * level.getTileSize();
            const int worldHeight = level.getHeight() * level.getTileSize();
            camera.follow(world.getPlayer(), worldWidth, worldHeight);
            const int offsetX = camera.getOffsetX();
            const int offsetY = camera.getOffsetY();

            SDL_RenderSetScale(
                renderer,
                static_cast<float>(CAMERA_ZOOM),
                static_cast<float>(CAMERA_ZOOM));
            // Vẽ background gameplay với hiệu ứng parallax.
            worldRenderer.renderScrollingBackground(
                renderer,
                *textureManager,
                static_cast<int>(WINDOW_WIDTH / CAMERA_ZOOM),
                static_cast<int>(WINDOW_HEIGHT / CAMERA_ZOOM),
                static_cast<int>(gameplayBackgroundOffset));
            worldRenderer.render(
                renderer, *textureManager, world, offsetX, offsetY);
            actorRenderer.renderEnemies(
                renderer, *textureManager, world, offsetX, offsetY);
            actorRenderer.renderEffects(
                renderer, *textureManager, offsetX, offsetY);
            actorRenderer.renderPlayer(
                renderer, *textureManager, world.getPlayer(), offsetX, offsetY);
            SDL_RenderSetScale(renderer, 1.0F, 1.0F);
            // Hiển thị thông tin gameplay trên HUD.
            hudRenderer.render(
                renderer, world.getScore(), world.getRemainingCoins(),
                world.getTimeRemaining(), world.getLives());
            if (const GorillaBoss* boss = world.getBoss()) {
                hudRenderer.renderBossHealth(
                    renderer, boss->getCurrentHp(), boss->getMaxHp(),
                    boss->getPhase());
            }
            break;
        }
        case Editing:{
            mapEditor->update();
            mapEditor->render(renderer, *textureManager);

            actorRenderer.updatePlayer(world.getPlayer(), deltaMs);
            actorRenderer.updateWorld(world, deltaMs);
            const SDL_Rect viewport = mapEditor->getMapViewport();
            SDL_RenderSetClipRect(renderer, &viewport);
            actorRenderer.renderEnemies(
                renderer,
                *textureManager,
                world,
                -mapEditor->getCameraX(),
                -mapEditor->getCameraY());
            actorRenderer.renderPlayer(
                renderer,
                *textureManager,
                world.getPlayer(),
                -mapEditor->getCameraX(),
                -mapEditor->getCameraY());
            SDL_RenderSetClipRect(renderer, nullptr);
        }
        break;
        case LevelComplete:
        case GameOver:
            SDL_RenderSetScale(
                renderer,
                static_cast<float>(CAMERA_ZOOM),
                static_cast<float>(CAMERA_ZOOM));
            // Giữ hiệu ứng parallax trên màn hình kết thúc.
            worldRenderer.renderScrollingBackground(
                renderer,
                *textureManager,
                static_cast<int>(WINDOW_WIDTH / CAMERA_ZOOM),
                static_cast<int>(WINDOW_HEIGHT / CAMERA_ZOOM),
                static_cast<int>(gameplayBackgroundOffset));
            worldRenderer.render(
                renderer, *textureManager, world,
                camera.getOffsetX(), camera.getOffsetY());
            actorRenderer.renderEnemies(
                renderer, *textureManager, world,
                camera.getOffsetX(), camera.getOffsetY());
            actorRenderer.renderEffects(
                renderer, *textureManager,
                camera.getOffsetX(), camera.getOffsetY());
            actorRenderer.renderPlayer(
                renderer,
                *textureManager,
                world.getPlayer(),
                camera.getOffsetX(),
                camera.getOffsetY());
            SDL_RenderSetScale(renderer, 1.0F, 1.0F);
            // Hiển thị kết quả và số mạng ở màn hình kết thúc.
            terminalScreen.render(
                renderer, currentGameState, world.getScore(), world.getLives());
            break;
        case Exit:
            break;
    }
    SDL_RenderPresent(renderer);
}
}
