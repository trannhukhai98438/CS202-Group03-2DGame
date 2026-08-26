#pragma once

#include "Hero.h"

class Flash : public Hero {
protected:
	bool thunderTextureAvailable{false};
	std::unique_ptr<Projectile> createSpecialProjectile() const override;

public:
	Flash(float x, float y, ProjectileSpawnCallback spawnCallback = nullptr);
	~Flash() = default;
	void loadTexture(const std::string& path) override;
	float getSpecialCooldown() const override { return 2.5f; }
};
