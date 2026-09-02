#include "Entities/Character/Enemy/Witch.h"
#include "Entities/Character/Enemy/Potion.h"
#include <iostream>

void ThrowState::onEnter(Enemy& enemy) {
    throwTimer = 0.0f;
    hasThrown = false;
    // Stop moving when throwing
}

void ThrowState::update(Enemy& enemy, float deltaTime) {
    throwTimer += deltaTime;
    Witch* witch = dynamic_cast<Witch*>(&enemy);
    
    // At 0.25 seconds, actually throw the potion
    if (throwTimer >= 0.25f && !hasThrown) {
        if (witch) {
            witch->throwPotion();
        }
        hasThrown = true;
    }
    
    // At end of duration, smoothly return to patrol
    if (throwTimer >= stateDuration) {
        enemy.changeState(std::make_unique<PatrolState>());
    }
}

void ThrowState::onExit(Enemy& enemy) {
    // Resume patrol logic
}


Witch::Witch(float startX, float startY, float patrolRange, std::function<void(std::unique_ptr<Projectile>)> spawnCallback)
    : Enemy(startX, startY, 30.0f, patrolRange), attackCooldown(0.0f), spawnProjectileCallback(std::move(spawnCallback)) {
    
    shape.setSize(sf::Vector2f(32.0f, 64.0f));
    shape.setFillColor(sf::Color::Blue);
    health = 1; // Witch dies in 1 stomp hit
    changeState(std::make_unique<PatrolState>());

    animatorComp.initAnimations(*this);
}

int Witch::getDamageOnTouch() const {
    if (getStateName() == "Squished" || getStateName() == "FlippingDeath") return 0;
    return 1;
}

void Witch::onStomped(Character* attacker) {
    physics.onStomped(*this, attacker);
}

void Witch::takeDamage(int damage) {
    physics.takeDamage(*this, damage);
}

void Witch::update(float deltaTime) {
    if (!isAlive) return;

    if (currentState) {
        currentState->update(*this, deltaTime);
    }
    
    if (getStateName() == "Squished" || getStateName() == "FlippingDeath") return;

    physics.updateCooldown(*this, deltaTime);

    move(deltaTime);
    checkObstacles();
    applyAnimation();
}

void Witch::checkObstacles() {
    physics.checkObstacles(*this);
}

void Witch::move(float deltaTime) {
    physics.move(*this, deltaTime);
}

void Witch::applyAnimation() {
    animatorComp.applyAnimation(*this);
    // Continuously lock sprite origin to hitbox bottom-center even when standing still
    sprite.setPosition(position.x + (shape.getSize().x / 2.0f), position.y + shape.getSize().y);
}

void Witch::throwPotion() {
    if (spawnProjectileCallback) {
        float throwDir = (currentDir == MoveDirection::Right) ? 1.0f : -1.0f;
        float startX = position.x + (shape.getSize().x / 2.0f);
        float startY = position.y;
        float velX = throwDir * 480.0f; // Throw further across the screen!
        float velY = -550.0f; // Smooth high arc!
        
        auto potion = std::make_unique<Potion>(startX, startY, velX, velY);
        spawnProjectileCallback(std::move(potion));
    }
}
