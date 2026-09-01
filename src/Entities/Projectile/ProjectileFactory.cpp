#include "Entities/Projectile/ProjectileFactory.h"
#include "Entities/Projectile/MarioFireball.h"
#include "Entities/Projectile/LuigiWaterBomb.h"
#include "Entities/Projectile/FlashThunder.h"

std::unique_ptr<Projectile> ProjectileFactory::createProjectile(
    const std::string& typeStr, 
    float x, 
    float y, 
    float vx, 
    float vy,
    ProjectileFaction faction) 
{
    if (typeStr == "MarioFireball" || typeStr == "Fireball") {
        auto proj = std::make_unique<MarioFireball>(x, y, vx);
        proj->setVelocity(vx, vy);
        return proj;
    }
    if (typeStr == "LuigiWaterBomb" || typeStr == "WaterBomb") {
        auto proj = std::make_unique<LuigiWaterBomb>(x, y, vx, vy);
        proj->setVelocity(vx, vy);
        return proj;
    }
    if (typeStr == "FlashThunder") {
        auto proj = std::make_unique<FlashThunder>(x, y, vx);
        proj->setVelocity(vx, vy);
        return proj;
    }

    return nullptr;
}