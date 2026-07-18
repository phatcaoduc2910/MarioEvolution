#pragma once

class Actor;
class GameObject;
class World;

/**
 * Điều phối va chạm gameplay của player với object, item và actor.
 */
class CollisionSystem {
public:
    bool check(const Actor& actor, const GameObject& object) const;
    void resolve(World& world);
};
