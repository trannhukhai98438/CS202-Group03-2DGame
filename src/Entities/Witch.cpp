#include "Entities/Witch.h"
#include "Entities/Potion.h"
#include <iostream>

void ThrowState::onEnter(Enemy& enemy) {
    throwTimer = 0.0f;
    hasThrown = false;
    // Stop moving when throwing
}

void ThrowState::update(Enemy& enemy, float deltaTime) {
    throwTimer += deltaTime;
    Witch* witch = dynamic_cast<Witch*>(&enemy);
    
    // At 0.5 seconds, actually throw the potion
    if (throwTimer >= 0.5f && !hasThrown) {
        if (witch) {
            witch->throwPotion();
        }
        hasThrown = true;
    }
    
    // At 1.0 seconds, return to patrol
    if (throwTimer >= stateDuration) {
        enemy.changeState(std::make_unique<PatrolState>());
    }
}

void ThrowState::onExit(Enemy& enemy) {
    // Resume patrol logic
}


Witch::Witch(float startX, float startY, float patrolRange, std::function<void(std::unique_ptr<Projectile>)> spawnCallback)
    : Enemy(startX, startY, 50.0f, patrolRange), animator(sprite), attackCooldown(0.0f), spawnProjectileCallback(std::move(spawnCallback)) {
    
    shape.setSize(sf::Vector2f(64.0f, 96.0f));
    shape.setFillColor(sf::Color::Blue);
    health = 2; // Witch takes 2 hits
    changeState(std::make_unique<PatrolState>());

    sf::Vector2i frameSize = loadSpriteTexture("assets/textures/witch.png", 6, 96.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;

        animator.addAnimation("walk", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight)
        }, 0.15f));
        animator.addAnimation("hold", Animation({
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight)
        }, 1.0f));
        animator.addAnimation("throw", Animation({
            sf::IntRect(frameWidth * 5, 0, frameWidth, frameHeight)
        }, 1.0f));
    }
}

int Witch::getDamageOnTouch() const {
    return 1;
}

void Witch::onStomped(Character* attacker) {
    (void)attacker;
    takeDamage(1);
}

void Witch::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        die();
    }
}

void Witch::update(float deltaTime) {
    if (!isAlive) return;

    if (currentState) {
        currentState->update(*this, deltaTime);
    }
    
    // Cooldown logic for throwing
    if (getStateName() == "Patrol") {
        attackCooldown += deltaTime;
        if (attackCooldown >= 3.0f) {
            attackCooldown = 0.0f;
            changeState(std::make_unique<ThrowState>(1.0f));
        }
    }

    move(deltaTime);
    checkObstacles();
    applyAnimation();
}

void Witch::checkObstacles() {
    if (getStateName() == "Throw") return; // Don't turn around while throwing
    Enemy::checkObstacles();
}

void Witch::move(float deltaTime) {
    if (getStateName() == "Throw") return; // Don't move while throwing
    Enemy::move(deltaTime);
}

void Witch::applyAnimation() {
    std::string sName = getStateName();
    if (sName == "Throw") {
        // We know it's a ThrowState, let's just use a simple timer-based approach
        // since we are holding the state for 1.0s. First 0.5s is hold, rest is throw.
        // Wait, attackCooldown continues counting in update? No, it only increments in Patrol.
        // So we need to track throw progress. Let's just use animator frame time directly.
        // Or simply: the ThrowState sets hasThrown at 0.5s.
        ThrowState* throwState = dynamic_cast<ThrowState*>(currentState.get());
        if (throwState && throwState->hasThrownFlag()) {
            animator.playAnimation("throw", 0.016f);
        } else {
            animator.playAnimation("hold", 0.016f);
        }
    } else {
        animator.playAnimation("walk", 0.016f);
    }
    
    // Flip sprite based on direction
    if (currentDir == MoveDirection::Right) {
        sprite.setScale(-std::abs(sprite.getScale().x), sprite.getScale().y);
    } else {
        sprite.setScale(std::abs(sprite.getScale().x), sprite.getScale().y);
    }
}

void Witch::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

void Witch::throwPotion() {
    if (spawnProjectileCallback) {
        float throwDir = (currentDir == MoveDirection::Right) ? 1.0f : -1.0f;
        float startX = position.x + (shape.getSize().x / 2.0f);
        float startY = position.y;
        float velX = throwDir * 300.0f;
        float velY = -400.0f; // Throw upwards
        
        auto potion = std::make_unique<Potion>(startX, startY, velX, velY);
        spawnProjectileCallback(std::move(potion));
    }
}
