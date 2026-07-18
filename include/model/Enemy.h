#pragma once

#include "Actor.h"

class Player;

/**
 * Actor thù địch có tốc độ tuần tra và trạng thái chiến đấu.
 */
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

/**
 * Enemy đi bộ đơn giản, chết khi bị giẫm.
 */
class Goomba : public Enemy {
public:
    Goomba(double x, double y);

    void patrol() override;
    void die() override;
};

/**
 * Enemy có thể chuyển giữa trạng thái đi bộ và mai rùa.
 */
class Koopa : public Enemy {
public:
    Koopa(double x, double y);

    void hideInShell();
    void kick();
    void patrol() override;

private:
    bool shellMode;
};
