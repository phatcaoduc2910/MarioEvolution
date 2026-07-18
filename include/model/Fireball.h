#pragma once

#include "Actor.h"

/**
 * Actor đạn bay ngang cho tới khi bị hủy.
 */
class Fireball : public Actor {
public:
    Fireball(double x, double y, Direction direction);

    void update() override;
    void destroy();
};
