#pragma once

#include <SDL2/SDL.h>

class InputHandler;

// Kết quả mà một màn hình gửi về cho Game sau khi xử lý đầu vào.
// Screen chỉ mô tả ý định; Game chịu trách nhiệm chuyển trạng thái thực tế.
enum class ScreenAction {
    None,
    StartGame,
    ExitGame,
    ResumeGame
};

class Screen {
public:
    virtual ~Screen() = default;

    virtual void render(SDL_Renderer* renderer) const = 0;
    virtual ScreenAction handleInput(InputHandler& input) = 0;
};
