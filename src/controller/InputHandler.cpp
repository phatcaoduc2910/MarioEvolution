#include "controller/InputHandler.h"
#include "SDL2/SDL.h"

bool InputHandler::isPressed(Key key) const {
    return pressedKeys.count(key) > 0;
}

void InputHandler::press(Key key) {
    pressedKeys.insert(key);
}

void InputHandler::release(Key key) {
    pressedKeys.erase(key);
}

//Gán key cho input
Key InputHandler::mapKey(SDL_Keycode keycode) const {
    switch (keycode) {
        case SDLK_LEFT:
        case SDLK_a:
            return Key::Left;
        case SDLK_RIGHT:
        case SDLK_d:
            return Key::Right;
        case SDLK_UP:
        case SDLK_w:
            return Key::Up;
        case SDLK_DOWN:
        case SDLK_s:
            return Key::Down;
        case SDLK_SPACE:
            return Key::Jump;
        case SDLK_f:
            return Key::Fire;
        case SDLK_p:
        case SDLK_ESCAPE:
            return Key::Esc;
        case SDLK_RETURN:
            return Key::Enter;
        case SDLK_0:
            return Key::Edit;
        default:
            return Key::None;
    }
}

void InputHandler::update() {}
