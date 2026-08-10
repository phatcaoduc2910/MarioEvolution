#pragma once

#include <SDL2/SDL.h>

class InputHandler;


/**
Giao diện render và xử lý input chung cho menu và pause screen.
Screen không tự thay đổi trạng thái Game; nó chỉ trả về ScreenAction.
*/
class Screen {
public:
    virtual ~Screen() = default;

    // Vẽ toàn bộ nội dung của màn hình
    virtual void render(SDL_Renderer* renderer) const = 0;

};
