#include "Entities/Goomba.h"

Goomba::Goomba(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 40.0f, patrolRange) {
    shape.setSize(sf::Vector2f(32.0f, 64.0f));
    shape.setFillColor(sf::Color(165, 42, 42)); // Brown color
    health = 1;
    changeState(std::make_unique<PatrolState>());

    animatorComp.initAnimations(*this);
}

int Goomba::getDamageOnTouch() const {
    if (getStateName() == "Squished" || getStateName() == "FlippingDeath") return 0;
    return 1;
}

bool Goomba::getIsSquished() const {
    return getStateName() == "Squished";
}

void Goomba::onStomped(BaseEntity* attacker) {
    physics.onStomped(*this, attacker);
}

void Goomba::takeDamage(int damage) {
    physics.takeDamage(*this, damage);
}

void Goomba::checkObstacles() {
    physics.checkObstacles(*this);
}

void Goomba::move(float deltaTime) {
    physics.move(*this, deltaTime);
}

void Goomba::applyAnimation() {
    animatorComp.applyAnimation(*this);
}

void Goomba::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
