#include "Entities/Koopa.h"

Koopa::Koopa(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 50.0f, patrolRange),
      shellSpeed(250.0f) {
    shape.setSize(sf::Vector2f(32.0f, 48.0f));
    shape.setFillColor(sf::Color(34, 139, 34));
    health = 2;
    changeState(std::make_unique<PatrolState>());
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

void Koopa::onStomped(Character* attacker) {
    (void)attacker;
    takeDamage(1);
}

void Koopa::onSideCollision(Character* attacker) {
    if (!attacker) return;
    if (getStateName() == "Shell") {
        MoveDirection kickDir = (attacker->getPosition().x < position.x) ? MoveDirection::Right : MoveDirection::Left;
        kickShell(kickDir);
    } else {
        attacker->takeDamage(getDamageOnTouch());
    }
}

void Koopa::takeDamage(int damage) {
    (void)damage;
    std::string sName = getStateName();
    if (sName == "Patrol") {
        shape.setSize(sf::Vector2f(32.0f, 32.0f));
        shape.setPosition(sf::Vector2f(position.x, position.y + 16.0f));
        changeState(std::make_unique<ShellState>());
    } else if (sName == "Shell") {
        kickShell(currentDir);
    } else if (sName == "SpinningShell") {
        die();
    }
}

void Koopa::kickShell(MoveDirection dir) {
    setDirection(dir);
    changeState(std::make_unique<SpinningShellState>());
}

void Koopa::checkObstacles() {
    if (getStateName() == "Shell") return;
    Enemy::checkObstacles();
}

void Koopa::move(float deltaTime) {
    if (getStateName() == "Shell") return;

    if (getStateName() == "SpinningShell") {
        float moveAmount = static_cast<float>(currentDir) * shellSpeed * deltaTime;
        position.x += moveAmount;
        shape.setPosition(position);
        return;
    }

    Enemy::move(deltaTime);
}

void Koopa::applyAnimation() {
    std::string sName = getStateName();
    if (sName == "SpinningShell") {
        shape.setFillColor(sf::Color(0, 255, 127));
    } else if (sName == "Shell") {
        shape.setFillColor(sf::Color(46, 139, 87));
    } else {
        shape.setFillColor(sf::Color(34, 139, 34));
    }
}

void Koopa::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    window.draw(shape);
}
