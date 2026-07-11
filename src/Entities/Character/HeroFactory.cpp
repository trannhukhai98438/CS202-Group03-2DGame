#include "HeroFactory.h"

std::unique_ptr<Character> HeroFactory::createHero(HeroType type, float startX, float startY){
    switch (type) {
            case HeroType::Mario:
                return std::make_unique<Mario>(startX, startY);
            default:
                return nullptr;
        }
}