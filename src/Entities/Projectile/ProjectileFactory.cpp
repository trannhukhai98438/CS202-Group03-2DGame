#include "Entities/Projectile/ProjectileFactory.h"
#include "Entities/Projectile/MarioFireball.h"
#include "Entities/Projectile/LuigiWaterBomb.h"
#include "Entities/Projectile/FlashThunder.h"
#include "Entities/Character/Enemy/Potion.h"

std::unique_ptr<Projectile> ProjectileFactory::createProjectile(
    const std::string& typeStr, 
    float x, 
    float y, 
    float vx, 
    float vy,
    ProjectileFaction faction) 
{
    std::unique_ptr<Projectile> projectile;

    if (typeStr == "MarioFireball" || typeStr == "Fireball" || typeStr == "fireball") {
        projectile = std::make_unique<MarioFireball>(x, y, vx);
    } else if (typeStr == "LuigiWaterBomb" || typeStr == "WaterBomb" || typeStr == "waterbomb") {
        projectile = std::make_unique<LuigiWaterBomb>(x, y, vx, vy);
    } else if (typeStr == "FlashThunder" || typeStr == "flashthunder") {
        projectile = std::make_unique<FlashThunder>(x, y, vx);
    } else if (typeStr == "Potion" || typeStr == "potion") {
        projectile = std::make_unique<Potion>(x, y, vx, vy);
    }

    if (!projectile) return nullptr;

    projectile->setVelocity(vx, vy);
    projectile->setFaction(faction);
    return projectile;
}
