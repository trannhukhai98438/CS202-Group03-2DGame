#include "CheerState.h"

#include "Hero.h"

void CheerState::enter(Hero* hero) {
    hero->setVelocity(0.f, 0.f);
}

void CheerState::update(Hero* hero, float) {
    hero->setVelocity(0.f, 0.f);
}

std::string CheerState::getState() {
    return "Cheer";
}
