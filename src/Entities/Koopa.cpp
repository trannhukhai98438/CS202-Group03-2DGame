#include "Entities/Koopa.h"

Koopa::Koopa(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 50.0f, patrolRange),
      shellSpeed(550.0f) {
    shape.setSize(sf::Vector2f(32.0f, 50.0f));
    shape.setFillColor(sf::Color(34, 139, 34));
    health = 2;
    changeState(std::make_unique<PatrolState>());

    animatorComp.initAnimations(*this);
}

float Koopa::getSpeed() const {
    if (getIsShellSpinning()) return shellSpeed;
    if (getIsShell()) return 0.0f;
    return speed;
}

int Koopa::getDamageOnTouch() const {
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

void Koopa::onStomped(BaseEntity* attacker) {
    physics.onStomped(*this, attacker);
}

void Koopa::onSideCollision(BaseEntity* attacker) {
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
