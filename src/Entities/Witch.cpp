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
    : Enemy(startX, startY, 50.0f, patrolRange), attackCooldown(0.0f), spawnProjectileCallback(std::move(spawnCallback)) {
    
    shape.setSize(sf::Vector2f(32.0f, 96.0f));
    shape.setFillColor(sf::Color::Blue);
    health = 1; // Witch dies in 1 stomp hit
    changeState(std::make_unique<PatrolState>());

    sf::Vector2i frameSize = loadSpriteTexture("assets/textures/witch.png", 6, 96.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;
        
        animator.addAnimation("walk", Animation({
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 5, 0, frameWidth, frameHeight)
        }, 0.2f));
        animator.addAnimation("hold", Animation({
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight)
        }, 1.0f));
        animator.addAnimation("throw", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight)
        }, 1.0f));
    }
}

int Witch::getDamageOnTouch() const {
    if (getStateName() == "FlippingDeath") return 0;
    return 1;
}

void Witch::onStomped(BaseEntity* attacker) {
    (void)attacker;
    takeDamage(1);
}

void Witch::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        changeState(std::make_unique<FlippingDeathState>());
    }
}

void Witch::update(float deltaTime) {
    if (!isAlive) return;

    if (currentState) {
        currentState->update(*this, deltaTime);
    }
    
    if (getStateName() == "FlippingDeath") return;

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
    if (getStateName() == "Throw" || getStateName() == "FlippingDeath") return;
    Enemy::checkObstacles();
}

void Witch::move(float deltaTime) {
    if (getStateName() == "Throw" || getStateName() == "FlippingDeath") return;
    Enemy::move(deltaTime);
}

void Witch::applyAnimation() {
    if (getStateName() == "FlippingDeath") return;

    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (currentDir == MoveDirection::Left) {
        sprite.setScale(absScaleX, absScaleY);
    } else {
        sprite.setScale(-absScaleX, absScaleY);
    }

    std::string sName = getStateName();
    if (sName == "Throw") {
        ThrowState* throwState = dynamic_cast<ThrowState*>(currentState.get());
        if (throwState && throwState->hasThrownFlag()) {
            animator.playAnimation("throw", 0.016f);
        } else {
            animator.playAnimation("hold", 0.016f);
        }
    } else {
        animator.playAnimation("walk", 0.016f);
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
