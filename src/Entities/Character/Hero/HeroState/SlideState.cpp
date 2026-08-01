#include "Hero.h"
#include "SlideState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "PhysicsConstants.h"
#include <cmath>

void SlideState::enter(Hero* hero){
    // No special setup — friction in update() will handle deceleration
}

void SlideState::update(Hero* hero, float deltatime){
    sf::Vector2f vel = hero->getVelocity();

    // --- Safety: if airborne, transition to Jump ---
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    // --- Heavy friction to stop quickly (1.5× normal) //temporary ---
    float friction = PhysicsConstants::FRICTION * 1.5f * deltatime;
    if (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
    else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);

    // --- Apply gravity //temporary ---
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // --- Integrate position //temporary ---
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position and calls setGrounded() //temporary

    // --- Transition: stopped → Idle ---
    if (std::abs(vel.x) < PhysicsConstants::STOP_THRESHOLD){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SlideState::getState(){
    return "Slide";
}
