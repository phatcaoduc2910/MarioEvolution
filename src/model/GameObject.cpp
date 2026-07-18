#include "model/GameObject.h"

/**
 * Khởi tạo game object với vị trí và kích thước vùng bao.
 *
 * @param x Tọa độ x ban đầu.
 * @param y Tọa độ y ban đầu.
 * @param width Chiều rộng vùng bao.
 * @param height Chiều cao vùng bao.
 */
GameObject::GameObject(double x, double y, int width, int height)
    : x(x), y(y), width(width), height(height) {}

/**
 * Tạo hình chữ nhật va chạm từ vị trí và kích thước hiện tại của object.
 *
 * @return Hình chữ nhật dùng cho kiểm tra va chạm.
 */
Rectangle GameObject::getBounds() const {
    return {x, y, width, height};
}

/**
 * @return Tọa độ x hiện tại của object.
 */
double GameObject::getX() const {
    return x;
}

/**
 * @return Tọa độ y hiện tại của object.
 */
double GameObject::getY() const {
    return y;
}

/**
 * @return Chiều rộng vùng bao của object.
 */
int GameObject::getWidth() const {
    return width;
}

/**
 * @return Chiều cao vùng bao của object.
 */
int GameObject::getHeight() const {
    return height;
}

/**
 * Hook cập nhật mặc định cho game object.
 *
 * Lớp con override hàm này khi cần hành vi theo từng frame.
 */
void GameObject::update() {}

/**
 * Hook render mặc định cho game object.
 *
 * Việc render được triển khai bởi lớp con hoặc hệ thống view có quyền truy cập
 * tài nguyên renderer.
 */
void GameObject::render() {}
