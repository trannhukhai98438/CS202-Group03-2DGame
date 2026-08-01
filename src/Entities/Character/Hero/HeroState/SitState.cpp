#include "Hero.h"
#include "SitState.h"
#include "IdleState.h"
#include "PhysicsConstants.h"

void SitState::enter(Hero* hero){
    // Resize hitbox to crouching size (Giant/Fire sit = 16×24)
    hero->setSize(16, 24);
    hero->setVelocity(0.f, hero->getVelocity().y); // stop horizontal movement
}

void SitState::exit(Hero* hero){
    // Restore standing hitbox when getting up (Giant/Fire = 16×32)
    hero->setSize(16, 32);
}

void SitState::update(Hero* hero, float deltatime){
    // Apply gravity so hero stays on ground //temporary
    sf::Vector2f vel = hero->getVelocity();
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;
    hero->setVelocity(0.f, vel.y); // force x=0 while sitting

    // Integrate position //temporary
    sf::Vector2f pos = hero->getPosition();
    pos.y += vel.y * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position and calls setGrounded() //temporary

    // Transition: release Down → stand up
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SitState::getState(){
    return "Sit";
}
