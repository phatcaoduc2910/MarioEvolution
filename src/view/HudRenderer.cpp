#include "view/HudRenderer.h"

#include "view/UiRenderer.h"

#include <string>
#include <string_view>

namespace {
constexpr SDL_Color kPanelColor{18, 27, 45, 205};
constexpr SDL_Color kScoreColor{255, 255, 255, 255};
constexpr SDL_Color kStateColor{255, 214, 66, 255};

std::string_view stateName(PlayerState state) {
    switch (state) {
        case PlayerState::Big:
            return "BIG";
        case PlayerState::Fire:
            return "FIRE";
        case PlayerState::Dead:
            return "DEAD";
        case PlayerState::Small:
        default:
            return "SMALL";
    }
}
}

/** Hiển thị score và trạng thái Player cố định theo màn hình. */
void HudRenderer::render(SDL_Renderer* renderer, int score,
                         PlayerState state) const {
    if (renderer == nullptr) {
        return;
    }

    UiRenderer::fillRect(renderer, {12, 12, 230, 72}, kPanelColor);
    UiRenderer::drawText(
        renderer,
        "SCORE: " + std::to_string(score),
        24,
        24,
        2,
        kScoreColor);
    UiRenderer::drawText(
        renderer,
        "STATE: " + std::string(stateName(state)),
        24,
        54,
        2,
        kStateColor);
}
