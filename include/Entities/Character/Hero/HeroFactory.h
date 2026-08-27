#pragma once
#include "Hero.h"
#include <memory>

enum class HeroType {
    Mario,
	Luigi,
	Flash
};

class HeroFactory {
public:
    static std::unique_ptr<Hero> createHero(HeroType type, float x, float y,
                                            ProjectileSpawnCallback spawnCallback = nullptr);
};
