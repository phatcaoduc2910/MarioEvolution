#pragma once

#include "core/Types.h"

#include <SDL2/SDL.h>
#include <set>

class InputHandler {
public:
    bool isPressed(Key key) const;
    void press(Key key);
    void release(Key key);
    bool mapKey(SDL_Keycode keycode, Key& key);
    void update();

private:
    std::set<Key> pressedKeys;
};
