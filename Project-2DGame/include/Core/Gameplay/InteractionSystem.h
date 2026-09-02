#pragma once

#include "Core/PhysicsEngine.h"

class GameWorld;
class Projectile;

class InteractionSystem {
private:
    PhysicsEngine m_physics;

    int resolveHeroItems(GameWorld& world);
    int resolveSpinningShells(GameWorld& world);
    int resolveHeroEnemies(GameWorld& world);
    int resolveProjectiles(GameWorld& world);
    int resolveProjectileTargets(GameWorld& world, Projectile& projectile);
    int resolveHeroGoals(GameWorld& world);

public:
    int resolve(GameWorld& world);
};
