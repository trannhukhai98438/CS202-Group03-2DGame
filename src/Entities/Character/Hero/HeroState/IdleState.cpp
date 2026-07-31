#include "Hero.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"

void IdleState::enter(Hero* hero){
    hero->setVelocity(0.f, 0.f);
}

void IdleState::update(Hero* hero, float deltatime){
    // Check input to transition to RunState or JumpState
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setState(std::make_unique<RunState>());
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){ // Assuming Up or Z is jump
        hero->setState(std::make_unique<JumpState>());
    }
}

std::string IdleState::getState(){
    return "Idle";
}
