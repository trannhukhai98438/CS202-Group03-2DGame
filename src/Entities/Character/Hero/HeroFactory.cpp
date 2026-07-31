#include "HeroFactory.h"
#include "Mario.h"
#include "Luigi.h"

std::unique_ptr<Hero> HeroFactory::createHero(HeroType type, float x, float y) {
    switch (type) {
        case HeroType::Mario:
            return std::make_unique<Mario>(x, y);
        case HeroType::Luigi:
            return std::make_unique<Luigi>(x, y);
        default:
            return std::make_unique<Mario>(x, y); // Fallback
    }
}
