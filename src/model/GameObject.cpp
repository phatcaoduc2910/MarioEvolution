#include "model/GameObject.h"

GameObject::GameObject(double x, double y, int width, int height)
    : x(x), y(y), width(width), height(height) {}

/**
 * Builds the collision rectangle from the object's current position and size.
 *
 * @return Rectangle used by collision checks.
 */
Rectangle GameObject::getBounds() const {
    return {x, y, width, height};
}

double GameObject::getX() const {
    return x;
}

double GameObject::getY() const {
    return y;
}

int GameObject::getWidth() const {
    return width;
}

int GameObject::getHeight() const {
    return height;
}

/**
 * Default update hook for game objects.
 *
 * Subclasses override this when they need per-frame behavior.
 */
void GameObject::update() {}

/**
 * Default render hook for game objects.
 *
 * Rendering is implemented by subclasses or view systems that have access to
 * renderer resources.
 */
void GameObject::render() {}
