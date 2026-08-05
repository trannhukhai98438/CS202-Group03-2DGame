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
    : Enemy(startX, startY, 50.0f, patrolRange), attackCooldown(0.0f), spawnProjectileCallback(std::move(spawnCallback)) {
    
    shape.setSize(sf::Vector2f(32.0f, 96.0f));
    shape.setFillColor(sf::Color::Blue);
    health = 1; // Witch dies in 1 stomp hit
    changeState(std::make_unique<PatrolState>());

    animatorComp.initAnimations(*this);
}

int Witch::getDamageOnTouch() const {
    if (getStateName() == "Squished" || getStateName() == "FlippingDeath") return 0;
    return 1;
}

void Witch::onStomped(BaseEntity* attacker) {
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
