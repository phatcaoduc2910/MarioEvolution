#pragma once

#include "Actor.h"

class Player;

class Enemy : public Actor {
public:
    Enemy(double x, double y, int width, int height);
    ~Enemy() override = default;

    virtual void patrol();
    virtual void die();
    virtual void damagePlayer(Player& player);

protected:
    int walkingSpeed;
    EnemyState state;
};

class Goomba : public Enemy {
public:
    Goomba(double x, double y);

    void patrol() override;
    void die() override;
};

class Koopa : public Enemy {
public:
    Koopa(double x, double y);

    void hideInShell();
    void kick();
    void patrol() override;

private:
    bool shellMode;
};
