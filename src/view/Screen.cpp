#include "view/Screen.h"

#include "controller/InputHandler.h"

void Screen::render() {}

void Screen::handleInput(InputHandler& input) {
    (void)input;
}

StartScreen::StartScreen()
    : selectedOption(0) {}

void StartScreen::displayMenu() {}

void StartScreen::render() {}

void StartScreen::handleInput(InputHandler& input) {
    (void)input;
}

PauseScreen::PauseScreen()
    : paused(false) {}

bool PauseScreen::isPaused() const {
    return paused;
}

void PauseScreen::display() {}

void PauseScreen::render() {}
