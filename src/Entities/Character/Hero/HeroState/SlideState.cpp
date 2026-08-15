#include "Hero.h"
#include "SlideState.h"
#include "IdleState.h"
#include "JumpState.h"
#include "PhysicsConstants.h"
#include <cmath>

void SlideState::enter(Hero* hero){
    // isFacingRight is intentionally NOT changed here.
    // The character slides in the direction they were already moving.
    // Direction is locked for the entire duration of the slide.
}

void SlideState::update(Hero* hero, float deltatime){
    // Ground states only valid when grounded
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>(AirEntry::Fell));
        return;
    }

    sf::Vector2f vel = hero->getVelocity();

    // Ground state: vel.y is always 0 (no gravity here, CollisionSystem handles vertical) 
    vel.y = PhysicsConstants::GRAVITY*deltatime;

    // SLIDE_FRICTION (250 px/s²) is much lower than normal FRICTION (800 px/s²)
    // → slide from WALK_SPEED (150 px/s) lasts ~0.6s → animation clearly visible 
    // Input is completely ignored during slide — no direction change allowed
    float friction = PhysicsConstants::SLIDE_FRICTION * deltatime;
    if      (vel.x > 0.f) vel.x = std::max(0.f, vel.x - friction);
    else if (vel.x < 0.f) vel.x = std::min(0.f, vel.x + friction);

    hero->setVelocity(vel.x, vel.y);

    // Update pos in Engine;
    // sf::Vector2f pos = hero->getPosition();
    //pos.x += vel.x * deltatime;
    //hero->setPosition(pos.x, pos.y);

    // Fully stopped → return to Idle
    if (std::abs(vel.x) < PhysicsConstants::STOP_THRESHOLD){
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string SlideState::getState(){
    return "Slide";
}
