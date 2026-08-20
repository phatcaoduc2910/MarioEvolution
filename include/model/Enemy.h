#pragma once

#include "Actor.h"

class Player;

class Enemy : public Actor {
public:
    Enemy(double x, double y, int width, int height);
    ~Enemy() override = default;

    virtual void patrol();
    virtual void die();
    virtual void onPlayerContact(Player& player);
    virtual void onStomped(Player& player);
    virtual bool isStompable() const;
    virtual bool shouldTurnAtEdge() const;
    virtual bool isDeadlyToEnemies() const;
    // Boss miễn nhiễm fireball nên luật này phải hỏi từng enemy.
    virtual bool takesFireballDamage() const;
    void reverseDirection();

    bool isRemovable() const override;

    void update(double dtSeconds) override;

protected:
    void tickDeath(double dtSeconds);

    double walkingSpeed;
    EnemyState state;
    double deathElapsedSeconds;
};

class Goomba : public Enemy {
public:
    Goomba(double x, double y);

    void patrol() override;
    void die() override;
};

class Koopa : public Enemy {
public:
    static constexpr int kWalkWidth = 32;
    static constexpr int kWalkHeight = 48;
    static constexpr int kShellHeight = 32;

    Koopa(double x, double y, KoopaColor color);

    KoopaColor getColor() const;
    bool isShell() const;
    bool isSlidingShell() const;

    void hideInShell();
    void kick(Direction slideDirection);
    // Koopa bị boss ném hoặc rơi từ trên: giữ nguyên quỹ đạo tới khi chạm đất.
    void throwWith(double throwVelocityX, double throwVelocityY);
    bool isAirborne() const;

    void patrol() override;
    void onPlayerContact(Player& player) override;
    void onStomped(Player& player) override;
    bool shouldTurnAtEdge() const override;
    bool isDeadlyToEnemies() const override;

private:
    KoopaColor color;
    bool thrown;
};

class PiranhaPlant : public Enemy {
public:
    static constexpr int kPlantWidth = 32;
    static constexpr int kPlantHeight = 64;

    PiranhaPlant(double x, double mouthY);

    PiranhaPhase getPhase() const;
    // Hazard của boss arena: mọc đúng một vòng rồi tự dọn.
    void activateOnce();
    bool isOneShot() const;

    void update(double dtSeconds) override;
    void applyGravity(double dtSeconds) override;
    void patrol() override;
    void onPlayerContact(Player& player) override;
    bool isStompable() const override;
    bool shouldTurnAtEdge() const override;

private:
    void applyRisenHeight();

    PiranhaPhase phase;
    double phaseElapsedSeconds;
    double mouthY;
    bool oneShot;
    bool hasRisen;
};
