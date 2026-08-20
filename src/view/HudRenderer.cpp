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

constexpr SDL_Color kBossEmptyColor{54, 40, 40, 235};
constexpr SDL_Color kBossPhase1Color{86, 196, 96, 255};
constexpr SDL_Color kBossPhase2Color{240, 158, 52, 255};
constexpr SDL_Color kBossPhase3Color{224, 68, 68, 255};
constexpr int kBossBarY = 20;
constexpr int kBossPipWidth = 34;
constexpr int kBossPipHeight = 18;
constexpr int kBossPipGap = 6;

SDL_Color bossPhaseColor(BossPhase phase) {
    switch (phase) {
        case BossPhase::Phase3:
        case BossPhase::Dead:
            return kBossPhase3Color;
        case BossPhase::Phase2:
            return kBossPhase2Color;
        case BossPhase::Phase1:
        default:
            return kBossPhase1Color;
    }
}
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

/** Thanh HP boss neo giữa mép trên, đổi màu theo phase để báo Enraged/Last HP. */
void HudRenderer::renderBossHealth(SDL_Renderer* renderer, int currentHp,
                                   int maxHp, BossPhase phase) const {
    if (renderer == nullptr || maxHp <= 0) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
        return;
    }

    const int barWidth = maxHp * kBossPipWidth + (maxHp - 1) * kBossPipGap;
    const int barX = (screenWidth - barWidth) / 2;
    const SDL_Color filled = bossPhaseColor(phase);

    UiRenderer::fillRect(
        renderer,
        {barX - 6, kBossBarY - 6, barWidth + 12, kBossPipHeight + 12},
        kBorderColor);
    UiRenderer::fillRect(
        renderer,
        {barX - 3, kBossBarY - 3, barWidth + 6, kBossPipHeight + 6},
        kPanelColor);

    for (int pip = 0; pip < maxHp; ++pip) {
        UiRenderer::fillRect(
            renderer,
            {barX + pip * (kBossPipWidth + kBossPipGap), kBossBarY,
             kBossPipWidth, kBossPipHeight},
            (pip < currentHp) ? filled : kBossEmptyColor);
    }

    const char* label = (phase == BossPhase::Phase3) ? "BOSS - LAST HP"
                        : (phase == BossPhase::Phase2) ? "BOSS - ENRAGED"
                                                       : "BOSS";
    UiRenderer::drawCenteredText(
        renderer, label, screenWidth, kBossBarY + kBossPipHeight + 12, 2,
        kStatusColor);
}
