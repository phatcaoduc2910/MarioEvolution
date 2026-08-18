#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>
#include <set>

class InputHandler {
public:
    bool isPressed(Key key) const;
    void press(Key key);
    void release(Key key);
    Key mapKey(SDL_Keycode keycode) const;

private:
    std::set<Key> pressedKeys;
};
