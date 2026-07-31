#include "Hero.h"
#include "FlyState.h"

void FlyState::enter(Hero* hero){
    // Lock inputs, prepare to move to next map
}

void FlyState::update(Hero* hero, float deltatime){
    // Auto move character, ignoring input
    // hero->setVelocity(100.f, 0.f); //temporary auto walk right
}

std::string FlyState::getState(){
    return "Fly";
}
