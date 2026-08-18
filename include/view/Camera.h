#pragma once

class GameObject;

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight, int edgePadding = 0);

    void follow(const GameObject& target, int worldWidth, int worldHeight);
    void reset();
    void resetTo(int leftEdgeX);

    int getX() const;
    int getY() const;
    int getOffsetX() const;
    int getOffsetY() const;

private:
    int viewportWidth;
    int viewportHeight;
    int edgePadding;
    int minimumX;
    int x;
    int y;
};
