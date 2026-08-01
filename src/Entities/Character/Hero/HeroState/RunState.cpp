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
    // --- Safety: Run only valid when grounded ---
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    sf::Vector2f vel = hero->getVelocity();
    bool movingLeft  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool movingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    // --- Horizontal acceleration ---
    if (movingLeft){
        hero->setFacingRight(false);
        vel.x -= PhysicsConstants::ACCELERATION * deltatime; //temporary
        if (vel.x < -PhysicsConstants::WALK_SPEED) vel.x = -PhysicsConstants::WALK_SPEED;
    }
    else if (movingRight){
        hero->setFacingRight(true);
        vel.x += PhysicsConstants::ACCELERATION * deltatime; //temporary
        if (vel.x > PhysicsConstants::WALK_SPEED) vel.x = PhysicsConstants::WALK_SPEED;
    }
    else {
        // No horizontal input — apply friction
        float friction = PhysicsConstants::FRICTION * deltatime; //temporary
        if (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
        else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);
    }

    // --- Apply gravity //temporary ---
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // --- Integrate position //temporary ---
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position and calls setGrounded() //temporary

    // --- State transitions ---

    // Jump
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    // Slide: moving in one direction but pressing the opposite key
    if (vel.x > PhysicsConstants::STOP_THRESHOLD && movingLeft){
        hero->setState(std::make_unique<SlideState>());
        return;
    }
    if (vel.x < -PhysicsConstants::STOP_THRESHOLD && movingRight){
        hero->setState(std::make_unique<SlideState>());
        return;
    }

    // Return to Idle when no input and velocity is negligible
    if (!movingLeft && !movingRight && std::abs(vel.x) < PhysicsConstants::STOP_THRESHOLD){
        hero->setState(std::make_unique<IdleState>());
        return;
    }
}

std::string RunState::getState(){
    return "Run";
}
