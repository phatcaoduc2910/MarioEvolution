#pragma once

class Actor;
class GameObject;
class World;

class CollisionSystem {
public:
    bool check(const Actor& actor, const GameObject& object) const;
    void resolve(World& world);
};
