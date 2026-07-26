#pragma once

#include "StaticObject.h"

class Player;

// Flag đại diện cho cột cờ cuối màn, là một vật thể đứng yên và có va chạm rắn
class Flag : public StaticObject {
public:
    Flag(double x, double y);

    bool isCaptured() const;
    void onCapture(Player& player);
    void showGameOverMessage();

private:
    /*
        Biến lưu trạng thái:
            false: người chơi chưa chạm được cờ
            true: cờ đã được chiếm, màn chơi hoàn thành
    */ 
    bool captured;
};
