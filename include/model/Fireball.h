#pragma once

#include "Actor.h"

class Fireball : public Actor {
public:
    Fireball(double x, double y, Direction direction);

    void update() override;
    void destroy();
};
