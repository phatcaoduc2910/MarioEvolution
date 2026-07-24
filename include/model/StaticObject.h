#pragma once

#include "GameObject.h"

class Actor;

class StaticObject : public GameObject {
public:
    StaticObject(double x, double y, int width, int height);
    ~StaticObject() override = default;

    bool isSolid() const;
    virtual void onCollision(Actor& actor);

protected:
    bool solid;
};
