#include "Hero.h"
#include "RunState.h"
#include "JumpState.h"
#include "SlideState.h"
#include "IdleState.h"
#include "PhysicsConstants.h"
#include <cmath>

void RunState::enter(Hero* hero){
    // No special setup needed on enter
}

void RunState::update(Hero* hero, float deltatime){
    // Ground states only valid when grounded
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    bool pressLeft   = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool pressRight  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool bothPressed = pressLeft && pressRight; // both at same time → cancel each other

    sf::Vector2f vel = hero->getVelocity();

    // Ground state: vel.y is always 0 (no gravity here, CollisionSystem handles vertical) //temporary
    vel.y = 0.f;

    // --- Horizontal movement ---
    if (bothPressed){
        // Both directions cancel → apply friction (no direction change, no slide)
        float friction = PhysicsConstants::FRICTION * deltatime; //temporary
        if      (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
        else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);
    }
    else if (pressLeft){
        hero->setFacingRight(false);
        vel.x -= PhysicsConstants::ACCELERATION * deltatime; //temporary
        if (vel.x < -PhysicsConstants::WALK_SPEED) vel.x = -PhysicsConstants::WALK_SPEED;
    }
    else if (pressRight){
        hero->setFacingRight(true);
        vel.x += PhysicsConstants::ACCELERATION * deltatime; //temporary
        if (vel.x > PhysicsConstants::WALK_SPEED) vel.x = PhysicsConstants::WALK_SPEED;
    }
    else{
        // No input — friction
        float friction = PhysicsConstants::FRICTION * deltatime; //temporary
        if      (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
        else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);
    }

    hero->setVelocity(vel.x, vel.y);

    // Only update X — Y position is fixed to ground //temporary
    sf::Vector2f pos = hero->getPosition();
    pos.x += vel.x * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects pos.x and calls setGrounded() //temporary

    // --- State transitions ---

    // Jump (not when both keys conflict)
    if (!bothPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    // Slide: pressing opposite of movement direction AND speed is high enough to be visible
    // SLIDE_THRESHOLD prevents micro-slides that are too short to see
    // Both-pressed case excluded: no flip-flop spinning
    if (!bothPressed){
        if (vel.x >  PhysicsConstants::SLIDE_THRESHOLD && pressLeft){
            hero->setState(std::make_unique<SlideState>());
            return;
        }
        if (vel.x < -PhysicsConstants::SLIDE_THRESHOLD && pressRight){
            hero->setState(std::make_unique<SlideState>());
            return;
        }
    }

    // Return to Idle when no real input and essentially stopped
    if (std::abs(vel.x) < PhysicsConstants::STOP_THRESHOLD){
        hero->setState(std::make_unique<IdleState>());
        return;
    }
}

std::string RunState::getState(){
    return "Run";
}
