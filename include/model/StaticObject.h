#pragma once

#include "GameObject.h"

// GameObject đứng yên và mặc định có va chạm rắn
class StaticObject : public GameObject {
public:
    StaticObject(double x, double y, int width, int height);
    ~StaticObject() override = default;

    // Tách Actor khỏi object là việc của CollisionSystem, object chỉ khai báo rắn.
    bool isSolid() const;

protected:
    bool solid; // Xác định object có va chạm rắn hay không
};
