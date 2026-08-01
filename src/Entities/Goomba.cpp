#include "Entities/Goomba.h"

Goomba::Goomba(float startX, float startY, float patrolRange)
    : Enemy(startX, startY, 40.0f, patrolRange) {
    shape.setSize(sf::Vector2f(32.0f, 64.0f));
    shape.setFillColor(sf::Color(165, 42, 42));
    health = 1;
    changeState(std::make_unique<PatrolState>());

    // Original Goomba uses 64.0f targetSize (which scales frameWidth to 64.0f)
    sf::Vector2i frameSize = loadSpriteTexture("assets/textures/goomba.png", 6, 64.0f);
    if (frameSize.x > 0 && frameSize.y > 0) {
        int frameWidth = frameSize.x;
        int frameHeight = frameSize.y;

        animator.addAnimation("walk", Animation({
            sf::IntRect(0, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 2, 0, frameWidth, frameHeight),
            sf::IntRect(frameWidth * 3, 0, frameWidth, frameHeight)
        }, 0.15f));
        animator.addAnimation("squished", Animation({
            sf::IntRect(frameWidth * 4, 0, frameWidth, frameHeight)
        }, 0.2f));
    }
}

int Goomba::getDamageOnTouch() const {
    if (getIsSquished()) return 0;
    return 1;
}

bool Goomba::getIsSquished() const {
    return getStateName() == "Squished";
}

void Goomba::onStomped(BaseEntity* attacker) {
    (void)attacker;
    takeDamage(1);
}

void Goomba::takeDamage(int damage) {
    (void)damage;
    if (!getIsSquished()) {
        shape.setSize(sf::Vector2f(64.0f, 32.0f));
        shape.setPosition(sf::Vector2f(position.x, position.y + 32.0f));
        sprite.setPosition(sf::Vector2f(position.x + 32.0f, position.y + 64.0f + m_spriteOffsetY));
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
        animator.playAnimation("squished", 0.016f);
    } else {
        shape.setFillColor(sf::Color(165, 42, 42));
        animator.playAnimation("walk", 0.016f);
    }
}

void Goomba::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
