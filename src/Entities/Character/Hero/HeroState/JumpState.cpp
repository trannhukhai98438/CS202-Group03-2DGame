#include "Hero.h"
#include "JumpState.h"
#include "IdleState.h"

void JumpState::enter(Hero* hero){
    // Set vertical velocity to a negative value to jump up
    //hero->setVelocity(hero->getVelocity().x, -300.f); //temporary
}

void JumpState::update(Hero* hero, float deltatime){
    // allow horizontal movement in air
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        hero->setFacingRight(false);
        // move left
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setFacingRight(true);
        // move right
    }

    // Check if grounded to transition back
    if (hero->isGrounded()) {
        hero->setState(std::make_unique<IdleState>());
    }
    // temporary check since we don't have isGrounded access directly if it's protected
    // assuming physics class updates it and hero provides a getter
}

std::string JumpState::getState(){
    return "Jump";
}
