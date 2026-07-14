#pragma once

#include "view/Screen.h"

class PauseScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    ScreenAction handleInput(InputHandler& input) override;
};
