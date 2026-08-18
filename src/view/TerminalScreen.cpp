#include "view/TerminalScreen.h"

#include "view/UiRenderer.h"

#include <string>

namespace {
constexpr SDL_Color kOverlayColor{18, 27, 45, 220};
constexpr SDL_Color kWinColor{255, 214, 66, 255};
constexpr SDL_Color kLoseColor{235, 82, 82, 255};
constexpr SDL_Color kTextColor{255, 255, 255, 255};
constexpr SDL_Color kGuideColor{194, 208, 231, 255};
}

/** Hiển thị kết quả, điểm số và điều hướng sau khi màn chơi kết thúc. */
void TerminalScreen::render(SDL_Renderer* renderer, GameState state,
                            int score, int lives) const {
    if (renderer == nullptr ||
        (state != LevelComplete && state != GameOver)) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
        return;
    }

    UiRenderer::fillRect(
        renderer, {0, 0, screenWidth, screenHeight}, kOverlayColor);

    const bool won = state == LevelComplete;
    const bool canRetry = !won && lives > 0;
    const int centerY = screenHeight / 2;
    UiRenderer::drawCenteredText(
        renderer,
        won ? "LEVEL COMPLETE" : canRetry ? "LIFE LOST" : "GAME OVER",
        screenWidth,
        centerY - 110,
        5,
        won ? kWinColor : kLoseColor);
    UiRenderer::drawCenteredText(
        renderer,
        "SCORE: " + std::to_string(score),
        screenWidth,
        centerY - 20,
        3,
        kTextColor);
    UiRenderer::drawCenteredText(
        renderer,
        "LIVES: " + std::to_string(lives),
        screenWidth,
        centerY + 25,
        2,
        kTextColor);
    UiRenderer::drawCenteredText(
        renderer,
        won ? "ENTER: PLAY AGAIN"
            : canRetry ? "ENTER: RETRY" : "ENTER: NEW GAME",
        screenWidth,
        centerY + 65,
        2,
        kGuideColor);
    UiRenderer::drawCenteredText(
        renderer, "ESC: MENU", screenWidth, centerY + 105, 2, kGuideColor);
}
