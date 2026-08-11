#pragma once

#include "Actor.h"

class Fireball : public Actor {
public:
    Fireball(double x, double y, Direction direction);

    void update(double dtSeconds) override;
    void destroy();
};
