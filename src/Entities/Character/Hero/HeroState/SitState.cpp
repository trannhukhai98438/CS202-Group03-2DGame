#include "Hero.h"
#include "SitState.h"
#include "IdleState.h"

void SitState::enter(Hero* hero){
    // When entering sit state, we might resize hitbox
    hero->setSize(16, 24); // Giant Sit size
}

void SitState::exit(Hero* hero){
    // Restore size when standing up (Giant/Fire are 16x32)
    hero->setSize(16, 32);
}

void SitState::update(Hero* hero, float deltatime){
    // If player stops pressing Down, return to Idle
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SitState::getState(){
    return "Sit";
}
