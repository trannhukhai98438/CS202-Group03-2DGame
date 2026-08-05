#include "Hero.h"
#include "FlyState.h"
#include "PhysicsConstants.h"

void FlyState::enter(Hero* hero){
    // Lock input — auto-walk right to next map
    hero->setFacingRight(true);
}

void FlyState::update(Hero* hero, float deltatime){
    // Auto-walk right, ignore all player input
    sf::Vector2f vel = hero->getVelocity();
    vel.x = PhysicsConstants::WALK_SPEED;
    vel.y=0;

    // no gravity vel.y += PhysicsConstants::GRAVITY * deltatime;

    hero->setVelocity(vel.x, vel.y);

    // no position here
    //sf::Vector2f pos = hero->getPosition();
    //pos += vel * deltatime;
    //hero->setPosition(pos.x, pos.y);
}

std::string FlyState::getState(){
    return "Fly";
}
