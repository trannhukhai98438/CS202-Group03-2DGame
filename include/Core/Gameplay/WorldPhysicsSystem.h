#pragma once

#include "Core/PhysicsEngine.h"

class GameWorld;

class WorldPhysicsSystem {
public:
    void update(GameWorld& world, float deltaTime);

private:
    void resolveHero(GameWorld& world, float deltaTime);
    void resolveItems(GameWorld& world, float deltaTime);
    void resolveEnemies(GameWorld& world, float deltaTime);
    void resolveProjectiles(GameWorld& world, float deltaTime);

    PhysicsEngine m_physics;
};
