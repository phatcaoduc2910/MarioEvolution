#pragma once

#include "view/Screen.h"

/**
 * Hiển thị lớp phủ tạm dừng và yêu cầu tiếp tục từ phím Pause hoặc Enter.
 */
class PauseScreen final : public Screen {
public:
    void render(SDL_Renderer* renderer) const override;
    ScreenAction handleInput(InputHandler& input) override;
};
