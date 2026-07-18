#pragma once

#include "core/Types.h"

/**
 * Cơ sở chung cho mọi thực thể có vị trí và kích thước trong World.
 */
class GameObject {
public:
    GameObject(double x, double y, int width, int height);
    virtual ~GameObject() = default;

    Rectangle getBounds() const;
    double getX() const;
    double getY() const;
    int getWidth() const;
    int getHeight() const;

    virtual void update();
    virtual void render();

protected:
    double x;
    double y;
    int width;
    int height;
};
