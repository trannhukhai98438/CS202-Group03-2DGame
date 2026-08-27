#pragma once
#include "Hero.h"

class Mario:public Hero{
protected:
    std::unique_ptr<Projectile> createSpecialProjectile() const override;
public:
    Mario(float x, float y, ProjectileSpawnCallback spawnCallback = nullptr);
    ~Mario()=default;
    float getSpecialCooldown() const override { return 2.0f; }
};
