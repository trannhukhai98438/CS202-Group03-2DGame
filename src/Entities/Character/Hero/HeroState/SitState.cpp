#include "Hero.h"
#include "SitState.h"
#include "IdleState.h"

void SitState::enter(Hero* hero){
    hero->setSize(16, 24);          // Crouching hitbox (Giant/Fire sit = 16×24)
    hero->setVelocity(0.f, 0.f);   // Stop all movement on entering sit
}

void SitState::exit(Hero* hero){
    hero->setSize(16, 32);          // Restore standing hitbox (Giant/Fire = 16×32)
}

void SitState::update(Hero* hero, float deltatime){
    // Ground state: no gravity, no position update needed
    // Character is fully stationary while sitting
    hero->setVelocity(0.f, 0.f);   // keep velocity at zero every frame (defensive) //temporary
    // Y position unchanged — already on the ground
    // TODO: CollisionSystem maintains grounded state //temporary

    // Transition: release Down → stand up
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SitState::getState(){
    return "Sit";
}
