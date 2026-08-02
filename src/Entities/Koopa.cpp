#include "Entities/Koopa.h"

Koopa::Koopa(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 50.0f, patrolRange),
      shellSpeed(250.0f) {
    shape.setSize(sf::Vector2f(32.0f, 96.0f));
    shape.setFillColor(sf::Color(34, 139, 34));
    health = 2;
    changeState(std::make_unique<PatrolState>());

    sf::Vector2i frameSize = loadSpriteTexture("assets/textures/koopa.png", 6, 50.0f);
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

void Koopa::becomeStaticShell() {
    if (getStateName() == "Patrol") {
        position.y += 64.0f; 
        shape.setSize(sf::Vector2f(32.0f, 32.0f));
        shape.setPosition(position);
    }
    changeState(std::make_unique<ShellState>());
}

void Koopa::onStomped(BaseEntity* attacker) {
    (void)attacker;
    if (!isAlive) return;

    std::string state = getStateName();
    
    if (state == "Patrol") {
        becomeStaticShell();
    } else if (state == "Shell" || state == "SpinningShell") {
        takeDamage(1); // 2nd stomp kills Koopa cleanly with FlippingDeathState!
    }
}

void Koopa::onSideCollision(BaseEntity* attacker) {
    if (!isAlive) return;

    std::string state = getStateName();
    
    if (state == "Shell") {
        MoveDirection kickDir = (attacker && attacker->getPosition().x < position.x) ? MoveDirection::Right : MoveDirection::Left;
        kickShell(kickDir);
    } else {
        if (attacker) {
            attacker->takeDamage(getDamageOnTouch());
        }
    }
}

void Koopa::takeDamage(int damage) {
    (void)damage;
    if (!isAlive) return;
    changeState(std::make_unique<FlippingDeathState>());
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
    Enemy::move(deltaTime);
}

void Koopa::applyAnimation() {
    if (getStateName() == "FlippingDeath") return;

    std::string sName = getStateName();
    float absScaleX = std::abs(sprite.getScale().x);
    float absScaleY = std::abs(sprite.getScale().y);

    if (sName != "Shell" && sName != "SpinningShell") {
        if (currentDir == MoveDirection::Right) {
            sprite.setScale(absScaleX, absScaleY);
        } else {
            sprite.setScale(-absScaleX, absScaleY);
        }
    }

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
