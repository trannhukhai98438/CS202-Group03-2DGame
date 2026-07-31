#include "Hero.h"
#include "DeadState.h"

void DeadState::enter(Hero* hero){
    // Disable collision
    // hero->setVelocity(0.f, -400.f); //temporary bounce up before falling
}

void DeadState::update(Hero* hero, float deltatime){
    // Gravity pulls character down out of screen
    // Input is completely ignored
}

std::string DeadState::getState(){
    return "Dead";
}
