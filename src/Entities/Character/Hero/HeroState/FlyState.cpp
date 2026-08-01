#include "Hero.h"
#include "FlyState.h"
#include "PhysicsConstants.h"

void FlyState::enter(Hero* hero){
    // Lock input — auto-walk right to next map
    hero->setFacingRight(true);
}

void FlyState::update(Hero* hero, float deltatime){
    // Auto-walk right, ignore all player input //temporary
    sf::Vector2f vel = hero->getVelocity();
    vel.x = PhysicsConstants::WALK_SPEED;

    // Apply gentle gravity //temporary
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // Integrate position //temporary
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position //temporary
}

std::string FlyState::getState(){
    return "Fly";
}
