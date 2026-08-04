#include "Hero.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"
#include "PhysicsConstants.h"
#include <cmath>

void IdleState::enter(Hero* hero){
    // Do NOT zero velocity — allow remaining momentum to bleed off via friction
}

void IdleState::update(Hero* hero, float deltatime){
    // Ground states only valid when grounded
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    sf::Vector2f vel = hero->getVelocity();

    // Ground state: vel.y is always 0 (no gravity here, CollisionSystem handles vertical) //temporary
    vel.y = 0.f;

    // Apply friction to bleed off horizontal velocity //temporary
    float friction = PhysicsConstants::FRICTION * deltatime;
    if      (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
    else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);

    hero->setVelocity(vel.x, vel.y);

    // Only update X — Y position is fixed to ground //temporary
    sf::Vector2f pos = hero->getPosition();
    pos.x += vel.x * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects pos.x and calls setGrounded() //temporary

    // --- Input transitions ---
    bool pressLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool pressRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    // If both Left + Right are pressed simultaneously → ignore both (no movement)
    if (pressLeft && !pressRight){
        hero->setFacingRight(false);                          // set direction BEFORE switching state
        hero->setState(std::make_unique<RunState>());
        return;
    }
    if (pressRight && !pressLeft){
        hero->setFacingRight(true);                           // set direction BEFORE switching state
        hero->setState(std::make_unique<RunState>());
        return;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        hero->setState(std::make_unique<JumpState>());
        return;
    }
}

std::string IdleState::getState(){
    return "Idle";
}
