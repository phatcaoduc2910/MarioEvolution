#include "model/StaticObject.h"

/*
    Khởi tạo object có va chạm rắn
    x, y: Tọa độ object
    width: Chiều rộng vùng va chạm
    height: Chiều cao vùng va chạm
*/
StaticObject::StaticObject(double x, double y, int width, int height)
    : GameObject(x, y, width, height),
      solid(true){}

// Trả về true nếu object đang chặn Actor, ngược lại là false
bool StaticObject::isSolid() const{
    return solid;
}