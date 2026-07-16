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

void InputHandler::update() {}
