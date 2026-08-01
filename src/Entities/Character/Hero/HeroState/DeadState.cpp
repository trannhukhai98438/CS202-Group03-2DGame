#include "Hero.h"
#include "DeadState.h"
#include "PhysicsConstants.h"

void DeadState::enter(Hero* hero){
    // Small upward bounce then fall — classic Mario NES death //temporary
    hero->setVelocity(0.f, PhysicsConstants::JUMP_FORCE * 0.75f);
    hero->setGrounded(false);
}

void DeadState::update(Hero* hero, float deltatime){
    // Input is completely ignored in DeadState

    sf::Vector2f vel = hero->getVelocity();

    // Apply gravity — pull hero down off screen //temporary
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // Integrate position — no collision correction (hero falls through floor) //temporary
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // NOTE: Intentionally bypasses CollisionSystem — dead hero should fall through
}

std::string DeadState::getState(){
    return "Dead";
}
