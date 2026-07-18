#include "controller/InputHandler.h"
#include "SDL2/SDL.h"

/**
 * Kiểm tra một phím logic có đang được giữ hay không.
 *
 * @param key Phím logic cần kiểm tra.
 * @return true nếu phím đã được nhấn và chưa được thả; ngược lại là false.
 */
bool InputHandler::isPressed(Key key) const {
    return pressedKeys.count(key) > 0;
}

/**
 * Đánh dấu một phím logic đang được giữ.
 *
 * @param key Phím logic vừa được nhấn.
 */
void InputHandler::press(Key key) {
    pressedKeys.insert(key);
}

/**
 * Xóa trạng thái giữ của một phím logic.
 *
 * @param key Phím logic vừa được thả.
 */
void InputHandler::release(Key key) {
    pressedKeys.erase(key);
}

/**
 * Chuyển mã phím SDL sang hành động logic của game.
 *
 * Các phím mũi tên và WASD cùng ánh xạ vào nhóm di chuyển tương ứng.
 *
 * @param keycode Mã phím nhận từ SDL.
 * @param key Nơi nhận phím logic khi ánh xạ thành công.
 * @return true nếu game hỗ trợ phím đã cho; ngược lại là false.
 */
bool InputHandler::mapKey(SDL_Keycode keycode, Key& key) {
    switch (keycode) {
        case SDLK_LEFT:
        case SDLK_a:
            key = Key::Left;
            return true;
        case SDLK_RIGHT:
        case SDLK_d:
            key = Key::Right;
            return true;
        case SDLK_UP:
        case SDLK_w:
            key = Key::Up;
            return true;
        case SDLK_DOWN:
        case SDLK_s:
            key = Key::Down;
            return true;
        case SDLK_SPACE:
            key = Key::Jump;
            return true;
        case SDLK_f:
            key = Key::Fire;
            return true;
        case SDLK_p:
        case SDLK_ESCAPE:
            key = Key::Pause;
            return true;
        case SDLK_RETURN:
            key = Key::Enter;
            return true;
        default:
            return false;
    }
}

/**
 * Cập nhật input theo frame.
 *
 * Trạng thái hiện được cập nhật trực tiếp từ SDL_KEYDOWN và SDL_KEYUP nên hàm
 * này chưa cần thực hiện thêm công việc.
 */
void InputHandler::update() {}
