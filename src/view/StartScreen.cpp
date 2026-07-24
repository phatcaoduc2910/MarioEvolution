#include "view/StartScreen.h"

#include "controller/InputHandler.h"
#include "view/UiRenderer.h"

namespace {
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

SDL_Color buttonColor(MenuOption option, MenuOption selectedOption) {
    return option == selectedOption ? kSelectedColor : kButtonColor;
}
}

void StartScreen::render(SDL_Renderer* renderer) const {
    if (renderer == nullptr) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    if (SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight) != 0) {
        return;
    }

    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;
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

    UiRenderer::fillRect(renderer, panel, kPanelColor);
    UiRenderer::fillRect(
        renderer,
        startButton,
        buttonColor(MenuOption::StartGame, selectedOption)
    );
    UiRenderer::fillRect(
        renderer,
        exitButton,
        buttonColor(MenuOption::Exit, selectedOption)
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

ScreenAction StartScreen::handleInput(InputHandler& input) {
    if (input.isPressed(Key::Up)) {
        selectedOption = MenuOption::StartGame;
    } else if (input.isPressed(Key::Down)) {
        selectedOption = MenuOption::Exit;
    }

    if (!input.isPressed(Key::Enter)) {
        return ScreenAction::None;
    }

    return selectedOption == MenuOption::StartGame
        ? ScreenAction::StartGame
        : ScreenAction::ExitGame;
}
