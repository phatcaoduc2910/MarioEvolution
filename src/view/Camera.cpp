#include "view/Camera.h"

#include "model/GameObject.h"

#include <algorithm>

Camera::Camera(int viewportWidth)
    : viewportWidth(viewportWidth), x(0) {}

void Camera::follow(const GameObject& target, int worldWidth) {
    const int targetCenterX =
        static_cast<int>(target.getX() + target.getWidth() / 2.0);
    const int desiredX = targetCenterX - viewportWidth / 2;
    const int maxX = std::max(0, worldWidth - viewportWidth);
    x = std::clamp(desiredX, 0, maxX);
}

void Camera::reset() {
    x = 0;
}

int Camera::getX() const {
    return x;
}

int Camera::getOffsetX() const {
    return -x;
}
