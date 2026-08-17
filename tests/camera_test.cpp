#include "view/Camera.h"

#include "model/GameObject.h"

#include <cassert>
#include <iostream>

int main() {
    Camera camera(800);

    GameObject nearStart(100.0, 0.0, 32, 48);
    camera.follow(nearStart, 1600);
    assert(camera.getX() == 0);

    GameObject inMiddle(800.0, 0.0, 32, 48);
    camera.follow(inMiddle, 1600);
    assert(camera.getX() == 416);
    assert(camera.getOffsetX() == -416);

    GameObject nearEnd(1550.0, 0.0, 32, 48);
    camera.follow(nearEnd, 1600);
    assert(camera.getX() == 800);

    camera.reset();
    assert(camera.getX() == 0);

    std::cout << "Camera follow validation passed\n";
    return 0;
}
