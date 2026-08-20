#include "view/Camera.h"

#include "model/GameObject.h"

#include <algorithm>

Camera::Camera(int viewportWidth, int viewportHeight, int edgePadding)
    : viewportWidth(viewportWidth),
      viewportHeight(viewportHeight),
      edgePadding(std::max(0, edgePadding)),
      minimumX(0),
      x(0),
      y(0) {}

// Cho cam theo player theo 2 trục và giữ viewport
void Camera::follow(const GameObject& target, int worldWidth, int worldHeight) {
    const int targetCenterX =
        static_cast<int>(target.getX() + target.getWidth() / 2.0);
    const int targetCenterY =
        static_cast<int>(target.getY() + target.getHeight() / 2.0);

    const int desiredX = targetCenterX - viewportWidth / 2;
    const int desiredY = targetCenterY - viewportHeight / 2;

    const int scrollableX = std::max(0, worldWidth - viewportWidth);
    const int scrollableY = std::max(0, worldHeight - viewportHeight);

    const int mapMinX = std::min(edgePadding, scrollableX / 2);
    const int minY = std::min(edgePadding, scrollableY);
    const int maxX = std::max(mapMinX, scrollableX - edgePadding);
    const int maxY = scrollableY;
    const int minX = std::clamp(minimumX, mapMinX, maxX);
    
    x = std::clamp(desiredX, minX, maxX);
    y = std::clamp(desiredY, minY, maxY);
}

void Camera::reset() {
    resetTo(0);
}

void Camera::resetTo(int leftEdgeX) {
    minimumX = std::max(0, leftEdgeX);
    x = minimumX;
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
