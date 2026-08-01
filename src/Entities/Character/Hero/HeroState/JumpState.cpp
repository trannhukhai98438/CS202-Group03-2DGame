#include "Hero.h"
#include "JumpState.h"
#include "IdleState.h"
#include "RunState.h"
#include "PhysicsConstants.h"
#include <cmath>

void JumpState::enter(Hero* hero){
    // Apply initial jump velocity //temporary
    hero->setVelocity(hero->getVelocity().x, PhysicsConstants::JUMP_FORCE);
    hero->setGrounded(false); //temporary — CollisionSystem will set true on landing
}

void JumpState::update(Hero* hero, float deltatime){
    sf::Vector2f vel = hero->getVelocity();

    // --- Horizontal air control (reduced compared to ground) //temporary ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        hero->setFacingRight(false);
        vel.x -= PhysicsConstants::ACCELERATION * PhysicsConstants::AIR_CONTROL * deltatime;
        if (vel.x < -PhysicsConstants::WALK_SPEED) vel.x = -PhysicsConstants::WALK_SPEED;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        hero->setFacingRight(true);
        vel.x += PhysicsConstants::ACCELERATION * PhysicsConstants::AIR_CONTROL * deltatime;
        if (vel.x > PhysicsConstants::WALK_SPEED) vel.x = PhysicsConstants::WALK_SPEED;
    }

    // --- Apply gravity //temporary ---
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // --- Integrate position //temporary ---
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position and calls setGrounded(true) on landing //temporary

    // --- Transition: return to ground state when landed ---
    if (hero->getGrounded()){
        if (std::abs(vel.x) < PhysicsConstants::STOP_THRESHOLD){
            hero->setState(std::make_unique<IdleState>());
        } else {
            hero->setState(std::make_unique<RunState>());
        }
    }
}

std::string JumpState::getState(){
    return "Jump";
}
