#include "view/PauseScreen.h"

#include "controller/InputHandler.h"
#include "view/UiRenderer.h"

namespace {
constexpr SDL_Color kOverlayColor{18, 27, 45, 215};
constexpr SDL_Color kTitleColor{255, 214, 66, 255};
constexpr SDL_Color kTextColor{255, 255, 255, 255};
}

void PauseScreen::render(SDL_Renderer* renderer) const {
    if (renderer == nullptr) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
        return;
    }

    UiRenderer::fillRect(
        renderer,
        {0, 0, screenWidth, screenHeight},
        kOverlayColor
    );

    const int centerY = screenHeight / 2;
    UiRenderer::drawCenteredText(
        renderer, "PAUSED", screenWidth, centerY - 70, 5, kTitleColor
    );
    UiRenderer::drawCenteredText(
        renderer, "PRESS P OR ENTER TO CONTINUE", screenWidth, centerY + 25, 2,
        kTextColor
    );
}

ScreenAction PauseScreen::handleInput(InputHandler& input) {
    if (input.isPressed(Key::Pause) || input.isPressed(Key::Enter)) {
        return ScreenAction::ResumeGame;
    }

    return ScreenAction::None;
}
