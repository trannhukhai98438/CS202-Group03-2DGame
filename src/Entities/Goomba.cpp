#include "Entities/Goomba.h"

Goomba::Goomba(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 40.0f, patrolRange) {
    shape.setSize(sf::Vector2f(32.0f, 32.0f));
    shape.setFillColor(sf::Color(165, 42, 42));
    health = 1;
    changeState(std::make_unique<PatrolState>());
}

int Goomba::getDamageOnTouch() const {
    if (getIsSquished()) return 0;
    return 1;
}

bool Goomba::getIsSquished() const {
    return getStateName() == "Squished";
}

void Goomba::onStomped(Character* attacker) {
    (void)attacker;
    takeDamage(1);
}

void Goomba::takeDamage(int damage) {
    (void)damage;
    if (!getIsSquished()) {
        shape.setSize(sf::Vector2f(32.0f, 16.0f));
        shape.setPosition(sf::Vector2f(position.x, position.y + 16.0f));
        changeState(std::make_unique<SquishedState>(0.5f));
    }
}

void Goomba::checkObstacles() {
    if (getIsSquished()) return;
    Enemy::checkObstacles();
}

void Goomba::move(float deltaTime) {
    if (getIsSquished()) return;
    Enemy::move(deltaTime);
}

void Goomba::applyAnimation() {
    if (getIsSquished()) {
        shape.setFillColor(sf::Color(100, 30, 30));
    } else {
        shape.setFillColor(sf::Color(165, 42, 42));
    }
}

void Goomba::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    window.draw(shape);
}
