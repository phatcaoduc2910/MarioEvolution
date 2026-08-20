#include "view/StartScreen.h"

#include "view/UiRenderer.h"

namespace{
    constexpr int kPanelWidth = 520;
    constexpr int kPanelHeight = 520;
    constexpr int kButtonWidth = 280;
    constexpr int kButtonHeight = 52;
    constexpr int kButtonGap = 16;

    constexpr SDL_Color kPanelColor{29, 44, 73, 235};
    constexpr SDL_Color kSelectedColor{242, 172, 45, 255};
    constexpr SDL_Color kButtonColor{69, 88, 120, 255};
    constexpr SDL_Color kTitleColor{255, 214, 66, 255};
    constexpr SDL_Color kTextColor{255, 255, 255, 255};
    constexpr SDL_Color kGuideColor{194, 208, 231, 255};

    /**
    Chọn màu nền của một nút theo trạng thái lựa chọn hiện tại.
    Nếu nút đang được chọn, trả về màu vàng (kSelectedColor)
    Nếu không, trả về màu xanh xám (kButtonColor)
    */
    struct MenuLayout {
        SDL_Rect panel;
        SDL_Rect previousLevel;
        SDL_Rect nextLevel;
        SDL_Rect start;
        SDL_Rect editor;
        SDL_Rect exit;
        int centerY;
    };

    // Tính vị trí các thành phần của menu bắt đầu.
    bool getLayout(SDL_Renderer* renderer, MenuLayout& layout) {
        int screenWidth = 0;
        int screenHeight = 0;
        if (renderer == nullptr ||
            SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
            return false;
        }

        const int centerX = screenWidth / 2;
        layout.centerY = screenHeight / 2;
        layout.panel = {
            centerX - kPanelWidth / 2,
            layout.centerY - kPanelHeight / 2,
            kPanelWidth,
            kPanelHeight};
        layout.previousLevel = {centerX - 205, layout.centerY - 132, 48, 40};
        layout.nextLevel = {centerX + 157, layout.centerY - 132, 48, 40};
        layout.start = {
            centerX - kButtonWidth / 2,
            layout.centerY - 66,
            kButtonWidth,
            kButtonHeight};
        layout.editor = {
            layout.start.x,
            layout.start.y + kButtonHeight + kButtonGap,
            kButtonWidth,
            kButtonHeight};
        layout.exit = {
            layout.start.x,
            layout.editor.y + kButtonHeight + kButtonGap,
            kButtonWidth,
            kButtonHeight};
        return true;
    }

    SDL_Color buttonColor(StartMenuAction action,
                          StartMenuAction selectedAction){
        return action == selectedAction ? kSelectedColor : kButtonColor;
    }

    bool contains(const SDL_Rect& rectangle, int x, int y) {
        const SDL_Point point{x, y};
        return SDL_PointInRect(&point, &rectangle) == SDL_TRUE;
    }
}

void StartScreen::render(SDL_Renderer* renderer) const {
    render(renderer, "NO MAP");
}

// Vẽ panel, bộ chọn map và các lựa chọn của màn hình bắt đầu.
void StartScreen::render(SDL_Renderer* renderer,
                         std::string_view levelName) const {
    MenuLayout layout;
    if (!getLayout(renderer, layout)) return;

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    UiRenderer::fillRect(renderer, layout.panel, kPanelColor);
    UiRenderer::fillRect(renderer, layout.previousLevel, kButtonColor);
    UiRenderer::fillRect(renderer, layout.nextLevel, kButtonColor);
    UiRenderer::fillRect(renderer, layout.start,
        buttonColor(StartMenuAction::StartGame, selectedAction));
    UiRenderer::fillRect(renderer, layout.editor,
        buttonColor(StartMenuAction::MapEditor, selectedAction));
    UiRenderer::fillRect(renderer, layout.exit,
        buttonColor(StartMenuAction::ExitGame, selectedAction));

    UiRenderer::drawCenteredText(
        renderer, "MARIO EVOLUTION", screenWidth, layout.centerY - 218, 4,
        kTitleColor
    );
    UiRenderer::drawCenteredText(
        renderer, "SELECT LEVEL", screenWidth, layout.centerY - 174, 2,
        kGuideColor
    );
    UiRenderer::drawCenteredText(
        renderer, levelName, screenWidth, layout.centerY - 121, 2, kTextColor
    );
    UiRenderer::drawText(
        renderer, "-", layout.previousLevel.x + 18,
        layout.previousLevel.y + 13, 2, kTextColor);
    UiRenderer::drawText(
        renderer, "+", layout.nextLevel.x + 18,
        layout.nextLevel.y + 13, 2, kTextColor);
    UiRenderer::drawCenteredText(
        renderer, "START GAME", screenWidth, layout.start.y + 16, 3, kTextColor
    );
    UiRenderer::drawCenteredText(
        renderer, "MAP EDITOR", screenWidth, layout.editor.y + 16, 3,
        kTextColor
    );
    UiRenderer::drawCenteredText(
        renderer, "EXIT", screenWidth, layout.exit.y + 16, 3, kTextColor
    );
    UiRenderer::drawCenteredText(
        renderer, "ARROWS OR CLICK - ENTER TO SELECT", screenWidth,
        layout.centerY + 218, 1, kGuideColor
    );
}

// Di chuyển lựa chọn trong menu theo hướng chỉ định.
void StartScreen::moveSelection(int direction) {
    constexpr int kActionCount = 3;
    int index = 0;
    if (selectedAction == StartMenuAction::MapEditor) index = 1;
    if (selectedAction == StartMenuAction::ExitGame) index = 2;
    index = (index + direction + kActionCount) % kActionCount;
    selectedAction = index == 0 ? StartMenuAction::StartGame
                   : index == 1 ? StartMenuAction::MapEditor
                                : StartMenuAction::ExitGame;
}

StartMenuAction StartScreen::getSelectedAction() const {
    return selectedAction;
}

// Xác định lựa chọn menu tại vị trí con trỏ.
StartMenuAction StartScreen::actionAt(SDL_Renderer* renderer,
                                      int x, int y) const {
    MenuLayout layout;
    if (!getLayout(renderer, layout)) return StartMenuAction::None;
    if (contains(layout.start, x, y)) return StartMenuAction::StartGame;
    if (contains(layout.editor, x, y)) return StartMenuAction::MapEditor;
    if (contains(layout.exit, x, y)) return StartMenuAction::ExitGame;
    return StartMenuAction::None;
}

// Xác định hướng chuyển màn tại vị trí con trỏ.
int StartScreen::levelDirectionAt(SDL_Renderer* renderer, int x, int y) const {
    MenuLayout layout;
    if (!getLayout(renderer, layout)) return 0;
    if (contains(layout.previousLevel, x, y)) return -1;
    if (contains(layout.nextLevel, x, y)) return 1;
    return 0;
}
