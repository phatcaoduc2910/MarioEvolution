#pragma once

#include "core/Types.h"

#include <set>

class InputHandler {
public:
    bool isPressed(Key key) const;
    void press(Key key);
    void release(Key key);
    void update();

private:
    std::set<Key> pressedKeys;
};
