#pragma once

#include <SDL2/SDL.h>

class InputHandler;

// Kết quả mà một màn hình trả về cho Game
enum class ScreenAction {
    None,           // Không thực hiện hành động
    StartGame,      // Bắt đầu gameplay
    ExitGame,       // Thoát trò chơi
    ResumeGame      // Tiếp tục gameplay sau khi tạm dừng
};

/**
Giao diện render và xử lý input chung cho menu và pause screen.
Screen không tự thay đổi trạng thái Game; nó chỉ trả về ScreenAction.
*/
class Screen {
public:
    virtual ~Screen() = default;

    // Vẽ toàn bộ nội dung của màn hình
    virtual void render(SDL_Renderer* renderer) const = 0;

    // Đọc trạng thái bàn phím từ InputHandler và trả về một ScreenAction
    virtual ScreenAction handleInput(InputHandler& input) = 0;
};
