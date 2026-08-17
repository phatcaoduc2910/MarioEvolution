#include "view/Camera.h"

#include "model/GameObject.h"

#include <cassert>
#include <iostream>

int main() {
    Camera camera(800, 600);

    GameObject nearStart(100.0, 100.0, 32, 48);
    camera.follow(nearStart, 1600, 1200);
    assert(camera.getX() == 0);
    assert(camera.getY() == 0);

    GameObject inMiddle(800.0, 600.0, 32, 48);
    camera.follow(inMiddle, 1600, 1200);
    assert(camera.getX() == 416);
    assert(camera.getY() == 324);
    assert(camera.getOffsetX() == -416);
    assert(camera.getOffsetY() == -324);

    GameObject nearEnd(1550.0, 1150.0, 32, 48);
    camera.follow(nearEnd, 1600, 1200);
    assert(camera.getX() == 800);
    assert(camera.getY() == 600);

    camera.reset();
    assert(camera.getX() == 0);
    assert(camera.getY() == 0);

    Camera paddedCamera(800, 600, 16);
    paddedCamera.follow(nearStart, 1600, 1200);
    assert(paddedCamera.getX() == 16);
    assert(paddedCamera.getY() == 16);
    paddedCamera.follow(nearEnd, 1600, 1200);
    assert(paddedCamera.getX() == 784);
    assert(paddedCamera.getY() == 600);

    std::cout << "Camera follow validation passed\n";
    return 0;
}
