#include "HeroFactory.h"
#include "Mario.h"
#include "Luigi.h"

std::unique_ptr<Hero> HeroFactory::createHero(HeroType type, float x, float y,
                                              ProjectileSpawnCallback spawnCallback) {
    switch (type) {
        case HeroType::Mario:
            return std::make_unique<Mario>(x, y, std::move(spawnCallback));
        case HeroType::Luigi:
            return std::make_unique<Luigi>(x, y, std::move(spawnCallback));
        default:
            return std::make_unique<Mario>(x, y, std::move(spawnCallback)); // Fallback
    }
}
