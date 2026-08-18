#pragma once

class Actor;
class GameObject;
class Player;
class StaticObject;
class World;

class CollisionSystem {
public:
    bool check(const Actor& actor, const GameObject& object) const;

    // gravity (Actor::update) -> moveX -> resolveX -> moveY -> resolveY -> onGround.
    void update(World& world, double dtSeconds);

private:
    void stepActor(Actor& actor, World& world, double dtSeconds) const;
    bool hasGroundAhead(const Actor& actor, const World& world) const;
    bool resolveX(Actor& actor, const World& world) const;
    void resolveY(Actor& actor, World& world) const;
    void hitBrickFromBelow(
        Player& player,
        StaticObject& object,
        World& world) const;
    void resolveInteractions(World& world) const;
    void resolveFireballHits(World& world) const;
    void resolveEnemyHits(World& world) const;
};
