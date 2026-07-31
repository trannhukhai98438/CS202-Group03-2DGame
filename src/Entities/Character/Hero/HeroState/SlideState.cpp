#include "Hero.h"
#include "SlideState.h"
#include "IdleState.h"
#include <cmath>

void SlideState::enter(Hero* hero){
    // Nothing special on enter, but movement input will be locked during update
}

void SlideState::update(Hero* hero, float deltatime){
    // Lock all movement keys
    // Friction will slow character down naturally

    // Wait until velocity X becomes 0 to transition back to Idle
    if (std::abs(hero->getVelocity().x) < 0.1f){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SlideState::getState(){
    return "Slide";
}
