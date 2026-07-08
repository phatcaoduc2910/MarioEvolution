#include "model/GameObject.h"

GameObject::GameObject(double x, double y, int width, int height)
    : x(x), y(y), width(width), height(height) {}

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

void GameObject::update() {}

void GameObject::render() {}
