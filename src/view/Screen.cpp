#include "view/Screen.h"

#include "controller/InputHandler.h"

#include <SDL2/SDL.h>

#include <array>
#include <cctype>
#include <string>
#include <unordered_map>

namespace {
using Glyph = std::array<unsigned char, 7>;

const std::unordered_map<char, Glyph> kFont = {
    {'A', {14, 17, 17, 31, 17, 17, 17}}, {'B', {30, 17, 17, 30, 17, 17, 30}},
    {'C', {14, 17, 16, 16, 16, 17, 14}}, {'D', {30, 17, 17, 17, 17, 17, 30}},
    {'E', {31, 16, 16, 30, 16, 16, 31}}, {'F', {31, 16, 16, 30, 16, 16, 16}},
    {'G', {14, 17, 16, 23, 17, 17, 15}}, {'H', {17, 17, 17, 31, 17, 17, 17}},
    {'I', {14, 4, 4, 4, 4, 4, 14}},      {'J', {7, 2, 2, 2, 2, 18, 12}},
    {'K', {17, 18, 20, 24, 20, 18, 17}}, {'L', {16, 16, 16, 16, 16, 16, 31}},
    {'M', {17, 27, 21, 21, 17, 17, 17}}, {'N', {17, 25, 21, 19, 17, 17, 17}},
    {'O', {14, 17, 17, 17, 17, 17, 14}}, {'P', {30, 17, 17, 30, 16, 16, 16}},
    {'Q', {14, 17, 17, 17, 21, 18, 13}}, {'R', {30, 17, 17, 30, 20, 18, 17}},
    {'S', {15, 16, 16, 14, 1, 1, 30}},   {'T', {31, 4, 4, 4, 4, 4, 4}},
    {'U', {17, 17, 17, 17, 17, 17, 14}}, {'V', {17, 17, 17, 17, 17, 10, 4}},
    {'W', {17, 17, 17, 21, 21, 21, 10}}, {'X', {17, 17, 10, 4, 10, 17, 17}},
    {'Y', {17, 17, 10, 4, 4, 4, 4}},     {'Z', {31, 1, 2, 4, 8, 16, 31}},
    {' ', {0, 0, 0, 0, 0, 0, 0}}
};

void fillRect(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y,
              int scale, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int cursorX = x;

    for (char rawCharacter : text) {
        const char character = static_cast<char>(
            std::toupper(static_cast<unsigned char>(rawCharacter))
        );
        const auto found = kFont.find(character);
        const Glyph& glyph = found != kFont.end() ? found->second : kFont.at(' ');

        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if ((glyph[row] & (1 << (4 - column))) != 0) {
                    SDL_Rect pixel{
                        cursorX + column * scale,
                        y + row * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        cursorX += 6 * scale;
    }
}

int centeredTextX(int screenWidth, const std::string& text, int scale) {
    return (screenWidth - static_cast<int>(text.size()) * 6 * scale) / 2;
}
}

void Screen::render(SDL_Renderer* renderer) {
    (void)renderer;
}

void Screen::handleInput(InputHandler& input) {
    (void)input;
}

StartScreen::StartScreen()
    : selectedOption(0) {}

int StartScreen::getSelectedOption() const {
    return selectedOption;
}

void StartScreen::displayMenu(SDL_Renderer* renderer) {
    if (renderer == nullptr) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    const SDL_Rect panel{screenWidth / 2 - 260, screenHeight / 2 - 190, 520, 380};
    const SDL_Rect startButton{screenWidth / 2 - 140, screenHeight / 2 - 20, 280, 60};
    const SDL_Rect exitButton{screenWidth / 2 - 140, screenHeight / 2 + 65, 280, 60};

    fillRect(renderer, panel, {29, 44, 73, 235});
    fillRect(renderer, startButton,
             selectedOption == 0 ? SDL_Color{242, 172, 45, 255}
                                 : SDL_Color{69, 88, 120, 255});
    fillRect(renderer, exitButton,
             selectedOption == 1 ? SDL_Color{242, 172, 45, 255}
                                 : SDL_Color{69, 88, 120, 255});

    const std::string title = "MARIO EVOLUTION";
    const std::string start = "START GAME";
    const std::string exit = "EXIT";
    const std::string guide = "USE ARROW KEYS AND ENTER";

    drawText(renderer, title, centeredTextX(screenWidth, title, 4),
             screenHeight / 2 - 135, 4, {255, 214, 66, 255});
    drawText(renderer, start, centeredTextX(screenWidth, start, 3),
             screenHeight / 2 - 1, 3, {255, 255, 255, 255});
    drawText(renderer, exit, centeredTextX(screenWidth, exit, 3),
             screenHeight / 2 + 84, 3, {255, 255, 255, 255});
    drawText(renderer, guide, centeredTextX(screenWidth, guide, 2),
             screenHeight / 2 + 150, 2, {194, 208, 231, 255});
}

void StartScreen::render(SDL_Renderer* renderer) {
    displayMenu(renderer);
}

void StartScreen::handleInput(InputHandler& input) {
    if (input.isPressed(Key::Up)) {
        selectedOption = 0;
    } else if (input.isPressed(Key::Down)) {
        selectedOption = 1;
    }

}

PauseScreen::PauseScreen()
    : paused(true) {}

bool PauseScreen::isPaused() const {
    return paused;
}

void PauseScreen::display(SDL_Renderer* renderer) {
    if (renderer == nullptr) {
        return;
    }

    int screenWidth = 0;
    int screenHeight = 0;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    fillRect(renderer, {0, 0, screenWidth, screenHeight}, {18, 27, 45, 215});

    const std::string title = "PAUSED";
    const std::string guide = "PRESS P OR ENTER TO CONTINUE";
    drawText(renderer, title, centeredTextX(screenWidth, title, 5),
             screenHeight / 2 - 70, 5, {255, 214, 66, 255});
    drawText(renderer, guide, centeredTextX(screenWidth, guide, 2),
             screenHeight / 2 + 25, 2, {255, 255, 255, 255});
}

void PauseScreen::render(SDL_Renderer* renderer) {
    if (paused) {
        display(renderer);
    }
}

void PauseScreen::handleInput(InputHandler& input) {
    if (input.isPressed(Key::Pause) || input.isPressed(Key::Enter)) {
        paused = false;
    }
}
