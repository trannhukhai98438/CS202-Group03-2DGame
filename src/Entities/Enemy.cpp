#include "Entities/Enemy.h"

Enemy::Enemy(float startX, float startY, float moveSpeed, float patrolRange)
    : Character(startX, startY),
      speed(moveSpeed),
      health(1),
      currentDir(MoveDirection::Left),
      startX(startX),
      patrolLeftBound(startX - patrolRange),
      patrolRightBound(startX + patrolRange),
      currentState(std::make_unique<PatrolState>()) {
}

void Enemy::changeState(std::unique_ptr<EnemyState> newState) {
    if (currentState) {
        currentState->onExit(*this);
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->onEnter(*this);
    }
}

void Enemy::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    window.draw(shape);
}

void Enemy::setPatrolBounds(float minX, float maxX) {
    patrolLeftBound = minX;
    patrolRightBound = maxX;
}

void Enemy::checkObstacles() {
    if (position.x <= patrolLeftBound) {
        setDirection(MoveDirection::Right);
    } else if (position.x >= patrolRightBound) {
        setDirection(MoveDirection::Left);
    }
}

void Enemy::move(float deltaTime) {
    float moveAmount = static_cast<float>(currentDir) * speed * deltaTime;
    position.x += moveAmount;
    shape.setPosition(position);
}

void Enemy::applyAnimation() {
}

void Enemy::interactWith(Character* other) {
    if (!other || !other->getIsAlive()) return;
    sf::FloatRect bounds = getBounds();
    sf::FloatRect otherBounds = other->getBounds();

    if (otherBounds.top + otherBounds.height <= bounds.top + 10.0f) {
        onStomped(other);
    } else {
        onSideCollision(other);
    }
}

void Enemy::onStomped(Character* attacker) {
    (void)attacker;
    die();
}

void Enemy::onSideCollision(Character* attacker) {
    if (!attacker) return;
    attacker->takeDamage(getDamageOnTouch());
}
