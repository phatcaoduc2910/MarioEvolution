#include "view/HudRenderer.h"

#include "view/UiRenderer.h"

#include <string>

namespace {
constexpr SDL_Color kPanelColor{18, 27, 45, 225};
constexpr SDL_Color kBorderColor{255, 214, 66, 255};
constexpr SDL_Color kScoreColor{255, 255, 255, 255};
constexpr SDL_Color kStatusColor{255, 214, 66, 255};
constexpr int kPanelX = 12;
constexpr int kPanelY = 12;
constexpr int kPanelWidth = 250;
constexpr int kPanelHeight = 138;
}

/** Hiển thị tiến độ màn chơi cố định theo màn hình. */
void HudRenderer::render(SDL_Renderer* renderer, int score,
                         int remainingCoins, int timeRemaining,
                         int lives) const {
    if (renderer == nullptr) {
        return;
    }

    UiRenderer::fillRect(
        renderer,
        {kPanelX, kPanelY, kPanelWidth, kPanelHeight},
        kBorderColor);
    UiRenderer::fillRect(
        renderer,
        {kPanelX + 3, kPanelY + 3, kPanelWidth - 6, kPanelHeight - 6},
        kPanelColor);
    UiRenderer::drawText(
        renderer,
        "SCORE: " + std::to_string(score),
        24,
        24,
        3,
        kScoreColor);
    UiRenderer::drawText(
        renderer,
        "COINS: " + std::to_string(remainingCoins),
        24,
        60,
        2,
        kStatusColor);
    UiRenderer::drawText(
        renderer, "TIME: " + std::to_string(timeRemaining),
        24, 86, 2, kStatusColor);
    UiRenderer::drawText(
        renderer, "LIVES: " + std::to_string(lives),
        24, 112, 2, kStatusColor);
}
