#include "Star.h"
#include "PhysicsConstants.h"
#include "Hero.h"

Star::Star(float x, float y) : Item(x, y), isSpawning(false), spawnStartY(y) {
    if (texture.loadFromFile("assets/textures/Star.png")) {
        sprite.setTexture(texture);
    }

    std::vector<sf::IntRect> starFrames;
    for (int i = 0; i < 4; ++i) {
        starFrames.push_back(sf::IntRect(i * 15 + 1, 1, 14, 16)); // FIX: starFrames (not coinFrames)
    }
    animator.addAnimation("Star", Animation(starFrames, 0.1f));
    animator.playAnimation("Star", 0.f);

    hitbox.setSize(sf::Vector2f(28.f, 32.f));
    hitbox.setFillColor(sf::Color::Yellow);
    sprite.setScale(2.f, 2.f);
    setPosition(x,y);
}

void Star::spawn() {
    isActive = true;
    isSpawning = true;
    spawnStartY = position.y;
    velocity.y  = -50.f;  // slowly move up
    velocity.x  = 0.f;    // no horizontal moving
}

std::unique_ptr<Item> Star::clone(Hero* hero) const {
    return std::make_unique<Star>(position.x, position.y);
}

void Star::update(float deltatime) {
    if (!isActive) return;

    if (isSpawning) {
        // Phase 1: moving up
        // Position update is handled by PlayingState
        if (spawnStartY - position.y >= 32.f) {
            position.y  = spawnStartY - 32.f;
            isSpawning  = false;
            velocity.y  = -450.f;  // bounce with high speed
            velocity.x  = 100.f;   // right moving
        }
    } else {
        // Phase 2: bouncing + moving
        if (!isGrounded){
            velocity.y += PhysicsConstants::GRAVITY * deltatime;  //temporary
            if (velocity.y>PhysicsConstants::MAX_FALL_SPEED) velocity.y=PhysicsConstants::MAX_FALL_SPEED;
        }

        // CollisionSystem sets setGrounded=true -> bounce back
        if (isGrounded) {
            velocity.y = PhysicsConstants::JUMP_FORCE;
            isGrounded = false;
        }
    }

    animator.playAnimation("Star", deltatime);
}

void Star::render(sf::RenderWindow& window) {
    if (isActive) window.draw(sprite);
}

void Star::getCollision() {
    velocity.x = -velocity.x; // bounce off wall
}

void Star::getCollected(Hero* hero) {
    if (!isActive || isSpawning) return;
    hero->setInvincible(10.0f, true); // true = starman rainbow effect
    isActive = false;
}

bool Star::isColliable(){
    return !isSpawning;
}