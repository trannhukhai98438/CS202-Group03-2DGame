#include "Entities/Character/Enemy/Koopa.h"

Koopa::Koopa(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 50.0f, patrolRange),
      shellSpeed(360.0f) {
    shape.setSize(sf::Vector2f(32.0f, 48.0f));
    shape.setFillColor(sf::Color(34, 139, 34));
    health = 2;
    changeState(std::make_unique<PatrolState>());

    animatorComp.initAnimations(*this);
}

void Koopa::update(float deltaTime) {
    if (kickCooldown > 0.0f) {
        kickCooldown -= deltaTime;
    }
    Enemy::update(deltaTime);
}

float Koopa::getSpeed() const {
    if (getIsShellSpinning()) return shellSpeed;
    if (getIsShell()) return 0.0f;
    return speed;
}

int Koopa::getDamageOnTouch() const {
    if (kickCooldown > 0.0f) return 0;
    if (getIsShellSpinning()) return 2;
    if (getIsShell()) return 0;
    return 1;
}

bool Koopa::getIsShell() const {
    std::string sName = getStateName();
    return sName == "Shell" || sName == "SpinningShell";
}

bool Koopa::getIsShellSpinning() const {
    return getStateName() == "SpinningShell";
}

void Koopa::becomeStaticShell() {
    physics.becomeStaticShell(*this);
}

void Koopa::onStomped(Character* attacker) {
    physics.onStomped(*this, attacker);
}

void Koopa::onSideCollision(Character* attacker) {
    physics.onSideCollision(*this, attacker);
}

void Koopa::takeDamage(int damage) {
    physics.takeDamage(*this, damage);
}

void Koopa::kickShell(MoveDirection dir) {
    physics.kickShell(*this, static_cast<int>(dir));
}

void Koopa::checkObstacles() {
    physics.checkObstacles(*this);
}

void Koopa::move(float deltaTime) {
    physics.move(*this, deltaTime);
}

void Koopa::applyAnimation() {
    animatorComp.applyAnimation(*this);
}

void Koopa::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
