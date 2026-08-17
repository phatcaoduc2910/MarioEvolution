#include "view/Camera.h"

#include "model/GameObject.h"

#include <algorithm>

Camera::Camera(int viewportWidth, int viewportHeight)
    : viewportWidth(viewportWidth), viewportHeight(viewportHeight), x(0), y(0) {}

void Camera::follow(const GameObject& target, int worldWidth, int worldHeight) {
    const int targetCenterX =
        static_cast<int>(target.getX() + target.getWidth() / 2.0);
    const int targetCenterY =
        static_cast<int>(target.getY() + target.getHeight() / 2.0);
    const int desiredX = targetCenterX - viewportWidth / 2;
    const int desiredY = targetCenterY - viewportHeight / 2;
    const int maxX = std::max(0, worldWidth - viewportWidth);
    const int maxY = std::max(0, worldHeight - viewportHeight);
    x = std::clamp(desiredX, 0, maxX);
    y = std::clamp(desiredY, 0, maxY);
}

void Camera::reset() {
    x = 0;
    y = 0;
}

int Camera::getX() const {
    return x;
}

int Camera::getY() const {
    return y;
}

int Camera::getOffsetX() const {
    return -x;
}

int Camera::getOffsetY() const {
    return -y;
}
