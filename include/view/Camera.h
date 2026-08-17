#pragma once

class GameObject;

class Camera {
public:
    explicit Camera(int viewportWidth);

    void follow(const GameObject& target, int worldWidth);
    void reset();

    int getX() const;
    int getOffsetX() const;

private:
    int viewportWidth;
    int x;
};
