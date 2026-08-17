#pragma once

class GameObject;

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight);

    void follow(const GameObject& target, int worldWidth, int worldHeight);
    void reset();

    int getX() const;
    int getY() const;
    int getOffsetX() const;
    int getOffsetY() const;

private:
    int viewportWidth;
    int viewportHeight;
    int x;
    int y;
};
