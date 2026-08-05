#include "Hero.h"
#include "SitState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "PhysicsConstants.h"

void SitState::enter(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(16, 24);          // Crouching hitbox (Giant/Fire sit = 16×24)
    hero->setVelocity(0.f, 0.f);   // Stop all movement on entering sit
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 24.f);
}

void SitState::exit(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(16, 32);          // Restore standing hitbox (Giant/Fire = 16×32)
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 32.f);
}

void SitState::update(Hero* hero, float deltatime){
    if (!hero->getGrounded()){
        sf::Vector2f vel=hero->getVelocity();
        hero->setState(std::make_unique<JumpState>());
        hero->setVelocity(vel.x,PhysicsConstants::GRAVITY*deltatime);
    }

    hero->setVelocity(0.f, PhysicsConstants::GRAVITY*deltatime);  
    // Y position unchanged — already on the ground

    // Transition: release Down → stand up
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SitState::getState(){
    return "Sit";
}
