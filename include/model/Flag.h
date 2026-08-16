#pragma once

#include "StaticObject.h"

class Player;

class Flag : public StaticObject {
public:
    static constexpr int kPoleWidth = 8;
    static constexpr int kPoleHeight = 160;

    Flag(double x, double y);

    bool isCaptured() const;
    void onCapture(Player& player);
    void showGameOverMessage();

private:
    bool captured;
};
