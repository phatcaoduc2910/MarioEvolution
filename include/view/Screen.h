#pragma once

#include <SDL2/SDL.h>

class InputHandler;

/**
 * Ý định mà Screen gửi về để Game thực hiện chuyển trạng thái.
 */
enum class ScreenAction {
    None,
    StartGame,
    ExitGame,
    ResumeGame
};

/**
 * Giao diện render và xử lý input chung cho menu và pause screen.
 *
 * Screen không tự thay đổi trạng thái Game; nó chỉ trả về ScreenAction.
 */
class Screen {
public:
    virtual ~Screen() = default;

    /**
     * Vẽ toàn bộ nội dung screen lên renderer hiện tại.
     *
     * @param renderer SDL renderer nhận lệnh vẽ.
     */
    virtual void render(SDL_Renderer* renderer) const = 0;

    /**
     * Đọc input và trả về hành động Game cần thực hiện.
     *
     * @param input Trạng thái input logic hiện tại.
     * @return Hành động mà Game cần xử lý.
     */
    virtual ScreenAction handleInput(InputHandler& input) = 0;
};
