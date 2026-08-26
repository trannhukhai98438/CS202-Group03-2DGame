#pragma once
#include "Hero.h"

class Luigi:public Hero{
protected:
    std::unique_ptr<Projectile> createSpecialProjectile() const override;
public:
    Luigi(float x, float y, ProjectileSpawnCallback spawnCallback = nullptr);
    ~Luigi()=default;
    float getSpecialCooldown() const override { return 3.0f; }
};
