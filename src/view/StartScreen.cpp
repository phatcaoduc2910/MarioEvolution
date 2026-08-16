#include "view/StartScreen.h"

#include "controller/InputHandler.h"
#include "view/UiRenderer.h"

namespace{
    constexpr int kPanelWidth = 520;
    constexpr int kPanelHeight = 380;
    constexpr int kButtonWidth = 280;
    constexpr int kButtonHeight = 60;

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
    SDL_Color buttonColor(Option option, Option selectedOption){
        return option == selectedOption ? kSelectedColor : kButtonColor;
    }
}

//Vẽ panel, các lựa chọn và hướng dẫn của màn hình bắt đầu.
void StartScreen::render(SDL_Renderer* renderer) const{
    // Kiểm tra renderer có hợp lệ không
    if (renderer == nullptr){
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;

    // Lấy kích thước cửa sổ bằng SDL_GetRendererOutputSize()
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0){
        return;
    }

    // Tính tâm màn hình
    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;

    // Tạo các hình chữ nhật: Panel Menu, nút Start và nút Exit
    const SDL_Rect panel{
        centerX - kPanelWidth / 2,
        centerY - kPanelHeight / 2,
        kPanelWidth,
        kPanelHeight
    };
    const SDL_Rect startButton{
        centerX - kButtonWidth / 2,
        centerY - 20,
        kButtonWidth,
        kButtonHeight
    };
    const SDL_Rect exitButton{
        centerX - kButtonWidth / 2,
        centerY + 65,
        kButtonWidth,
        kButtonHeight
    };

    // Vẽ panel và hai nút bằng fillRect()
    UiRenderer::fillRect(renderer, panel, kPanelColor);
    UiRenderer::fillRect(
        renderer,
        startButton,
        buttonColor(Option::StartGame, selectedOption)
    );
    UiRenderer::fillRect(
        renderer,
        exitButton,
        buttonColor(Option::ExitGame, selectedOption)
    );

    UiRenderer::drawCenteredText(
        renderer, "MARIO EVOLUTION", screenWidth, centerY - 135, 4, kTitleColor
    );
    UiRenderer::drawCenteredText(
        renderer, "START GAME", screenWidth, centerY - 1, 3, kTextColor
    );
    UiRenderer::drawCenteredText(
        renderer, "EXIT", screenWidth, centerY + 84, 3, kTextColor
    );
    UiRenderer::drawCenteredText(
        renderer, "USE ARROW KEYS AND ENTER", screenWidth, centerY + 150, 2,
        kGuideColor
    );
}
void StartScreen::setOption(Option option) {
    selectedOption = option;
}
