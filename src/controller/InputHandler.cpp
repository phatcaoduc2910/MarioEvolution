#include "controller/InputHandler.h"

bool InputHandler::isPressed(Key key) const {
    return pressedKeys.count(key) > 0;
}

void InputHandler::press(Key key) {
    pressedKeys.insert(key);
}

void InputHandler::release(Key key) {
    pressedKeys.erase(key);
}

void InputHandler::update() {}
