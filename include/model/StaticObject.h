#pragma once

#include "GameObject.h"

class Actor;

// GameObject đứng yên và mặc định có va chạm rắn
class StaticObject : public GameObject {
public:
    StaticObject(double x, double y, int width, int height);
    ~StaticObject() override = default;

    bool isSolid() const;   // Trả về object hiện có chặn Actor không
    virtual void onCollision(Actor& actor);

protected:
    bool solid; // Xác định object có va chạm rắn hay không
};
