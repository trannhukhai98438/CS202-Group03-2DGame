#include "Hero.h"
#include "RunState.h"
#include "JumpState.h"
#include "SlideState.h"
#include "IdleState.h"
#include <cmath>

void RunState::enter(Hero* hero){
    // Do nothing special on enter
}

void RunState::update(Hero* hero, float deltatime){
    //temporary physics constants
    //float acceleration = 500.f; //temporary

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        hero->setFacingRight(false);
        // move left
        //hero->setVelocity(-100.f, hero->getVelocity().y); //temporary
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setFacingRight(true);
        // move right
        //hero->setVelocity(100.f, hero->getVelocity().y); //temporary
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    // Check slide: if moving right but pressing left, or vice versa
    // (Logic handled by physics/input class usually, but if we do it here:)
    if (hero->getVelocity().x > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        hero->setState(std::make_unique<SlideState>());
        return;
    }
    else if (hero->getVelocity().x < 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setState(std::make_unique<SlideState>());
        return;
    }

    // Transition to idle if velocity is 0
    if (std::abs(hero->getVelocity().x) < 0.1f && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string RunState::getState(){
    return "Run";
}
