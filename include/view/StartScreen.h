#pragma once

#include "view/Screen.h"

enum class MenuOption {
    StartGame,
    Exit
};

class StartScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    ScreenAction handleInput(InputHandler& input) override;

private:
    MenuOption selectedOption{MenuOption::StartGame};
};
