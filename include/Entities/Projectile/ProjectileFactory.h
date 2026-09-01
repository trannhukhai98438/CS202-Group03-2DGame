#pragma once
#include <memory>
#include <string>
#include "Entities/Character/Enemy/Projectile.h"

class ProjectileFactory {
public:
    static std::unique_ptr<Projectile> createProjectile(
        const std::string& typeStr, 
        float x, 
        float y, 
        float vx, 
        float vy,
        ProjectileFaction faction
    );
};