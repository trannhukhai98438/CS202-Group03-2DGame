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
        hero->setState(std::make_unique<JumpState>(AirEntry::Fell));
        return;
    }

    sf::Vector2f vel = hero->getVelocity();


    vel.y = PhysicsConstants::GRAVITY*deltatime;

    // Apply friction to bleed off horizontal velocity
    float friction = PhysicsConstants::FRICTION * deltatime;
    if      (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
    else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);

    hero->setVelocity(vel.x, vel.y);

    // no update pos here
    //sf::Vector2f pos = hero->getPosition();
    //pos.x += vel.x * deltatime;
    //hero->setPosition(pos.x, pos.y);

    // --- Input transitions ---
    bool pressLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool pressRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        hero->setState(std::make_unique<JumpState>(AirEntry::Jumped));
        return;
    }
}

std::string IdleState::getState(){
    return "Idle";
}
