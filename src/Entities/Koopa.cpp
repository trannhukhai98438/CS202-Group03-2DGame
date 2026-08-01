#include "Entities/Koopa.h"

Koopa::Koopa(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 50.0f, patrolRange),
      shellSpeed(250.0f) {
    shape.setSize(sf::Vector2f(64.0f, 96.0f));
    shape.setFillColor(sf::Color(34, 139, 34));
    health = 2;
    changeState(std::make_unique<PatrolState>());

    sf::Vector2i frameSize = loadSpriteTexture("assets/textures/koopa.png", 6, 96.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;

        animator.addAnimation("walk", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight)
        }, 0.15f));
        animator.addAnimation("shell", Animation({
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight)
        }, 0.2f));
        animator.addAnimation("spin", Animation({
            sf::IntRect(frameWidth * 5, 0, frameWidth, frameHeight)
        }, 0.1f));
    }
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
        shape.setSize(sf::Vector2f(64.0f, 64.0f));
        shape.setPosition(sf::Vector2f(position.x, position.y + 32.0f));
        sprite.setPosition(sf::Vector2f(position.x + 32.0f, position.y + 96.0f + m_spriteOffsetY));
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
        sprite.setPosition(sf::Vector2f(position.x + shape.getSize().x / 2.0f, position.y + shape.getSize().y + m_spriteOffsetY));
        return;
    }

    Enemy::move(deltaTime);
}

void Koopa::applyAnimation() {
    std::string sName = getStateName();
    if (sName == "SpinningShell") {
        shape.setFillColor(sf::Color(0, 255, 127));
        animator.playAnimation("spin", 0.016f);
    } else if (sName == "Shell") {
        shape.setFillColor(sf::Color(46, 139, 87));
        animator.playAnimation("shell", 0.016f);
    } else {
        shape.setFillColor(sf::Color(34, 139, 34));
        animator.playAnimation("walk", 0.016f);
    }
}

void Koopa::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
