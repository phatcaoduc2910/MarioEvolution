#pragma once

#include "view/Screen.h"

/**
 * Các lựa chọn có thể chọn trên màn hình bắt đầu.
 */
enum class MenuOption {
    StartGame,
    Exit
};

/**
 * Menu đầu game điều hướng bằng phím lên, xuống và Enter.
 */
class StartScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    ScreenAction handleInput(InputHandler& input) override;

private:
    MenuOption selectedOption{MenuOption::StartGame};
};
