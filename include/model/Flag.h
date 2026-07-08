#pragma once

#include "StaticObject.h"

class Player;

class Flag : public StaticObject {
public:
    Flag(double x, double y);

    bool isCaptured() const;
    void onCapture(Player& player);
    void showGameOverMessage();

private:
    bool captured;
};
