#include "view/PauseScreen.h"

#include "controller/InputHandler.h"
#include "view/UiRenderer.h"

namespace {
constexpr SDL_Color kOverlayColor{18, 27, 45, 215};
constexpr SDL_Color kTitleColor{255, 214, 66, 255};
constexpr SDL_Color kTextColor{255, 255, 255, 255};
}

// Vẽ lớp phủ tạm dừng theo kích thước hiện tại của renderer.
void PauseScreen::render(SDL_Renderer* renderer) const {
    // Kiểm tra renderer
    if (renderer == nullptr) {
        return;
    }

    // Lấy kích thước renderer
    int screenWidth = 0;
    int screenHeight = 0;
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
        return;
    }

    // Vẽ hình chữ nhật phủ đầy màn hình
    UiRenderer::fillRect(
        renderer,
        {0, 0, screenWidth, screenHeight},
        kOverlayColor
    );

    // Vẽ chữ "PAUSED" và "PRESS P OR ENTER TO CONTINUE"
    const int centerY = screenHeight / 2;
    UiRenderer::drawCenteredText(
        renderer, "PAUSED", screenWidth, centerY - 175, 5, kTitleColor
    );
    UiRenderer::drawCenteredText(
        renderer, "PRESS ESC TO CONTINUE", screenWidth, centerY + 150, 2,
        kTextColor
    );
}
