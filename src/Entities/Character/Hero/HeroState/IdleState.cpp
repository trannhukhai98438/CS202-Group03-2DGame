#include "Hero.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"
#include "PhysicsConstants.h"

void IdleState::enter(Hero* hero){
    // Do NOT zero velocity here — allow momentum to bleed off naturally via friction
}

void IdleState::update(Hero* hero, float deltatime){
    // --- Safety: Idle only valid when grounded ---
    if (!hero->getGrounded()){
        hero->setState(std::make_unique<JumpState>());
        return;
    }

    sf::Vector2f vel = hero->getVelocity();

    // Apply friction to bleed off horizontal velocity //temporary
    float friction = PhysicsConstants::FRICTION * deltatime;
    if (vel.x > 0.f) {
        vel.x = std::max(0.f, vel.x - friction);
    } else if (vel.x < 0.f) {
        vel.x = std::min(0.f, vel.x + friction);
    }

    // Apply gravity //temporary
    vel.y += PhysicsConstants::GRAVITY * deltatime;
    if (vel.y > PhysicsConstants::MAX_FALL_SPEED) vel.y = PhysicsConstants::MAX_FALL_SPEED;

    hero->setVelocity(vel.x, vel.y);

    // Integrate position //temporary
    sf::Vector2f pos = hero->getPosition();
    pos += vel * deltatime;
    hero->setPosition(pos.x, pos.y);
    // TODO: CollisionSystem corrects position and calls setGrounded() each frame //temporary

    // --- Input transitions ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
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
