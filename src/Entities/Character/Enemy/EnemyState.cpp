#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Enemy/Enemy.h"

void PatrolState::update(Enemy& enemy, float deltaTime) {
    enemy.checkObstacles();
    enemy.move(deltaTime);
    enemy.applyAnimation();
}

SquishedState::SquishedState(float duration, float fadeTime) 
    : timer(duration), fadeDuration(fadeTime) {}

void SquishedState::onEnter(Enemy& enemy) {
    enemy.getSprite().setColor(sf::Color(255, 255, 255, 255));
}

void SquishedState::update(Enemy& enemy, float deltaTime) {
    timer -= deltaTime;
    enemy.applyAnimation();

    if (timer <= fadeDuration && fadeDuration > 0.0f) {
        float alphaRatio = std::max(0.0f, timer / fadeDuration);
        sf::Uint8 alpha = static_cast<sf::Uint8>(alphaRatio * 255.0f);
        enemy.getSprite().setColor(sf::Color(255, 255, 255, alpha));
    }

    if (timer <= 0.0f) {
        enemy.die();
    }
}

void ShellState::onEnter(Enemy& enemy) {
    (void)enemy;
}

void ShellState::update(Enemy& enemy, float deltaTime) {
    (void)deltaTime;
    enemy.applyAnimation();
}

void SpinningShellState::onEnter(Enemy& enemy) {
    (void)enemy;
}

void SpinningShellState::update(Enemy& enemy, float deltaTime) {
    enemy.checkObstacles();
    enemy.move(deltaTime);
    enemy.applyAnimation();
}

FlippingDeathState::FlippingDeathState(float initialVelY) : velocityY(initialVelY) {}

void FlippingDeathState::onEnter(Enemy& enemy) {
    sf::Vector2f scale = enemy.getSprite().getScale();
    enemy.getSprite().setScale(scale.x, -std::abs(scale.y));
}

void FlippingDeathState::update(Enemy& enemy, float deltaTime) {
    velocityY += 450.0f * deltaTime; // Soft, smooth gravity acceleration
    sf::Vector2f pos = enemy.getPosition();
    enemy.setPosition(sf::Vector2f(pos.x, pos.y + velocityY * deltaTime));
    
    // Quick cleanup if fallen below screen bounds
    if (pos.y > 800.0f) {
        enemy.die();
        return;
    }

    // Fade out
    sf::Sprite& sprite = enemy.getSprite();
    sf::Color color = sprite.getColor();
    if (color.a > 5) {
        color.a -= static_cast<sf::Uint8>(120 * deltaTime);
        sprite.setColor(color);
    } else {
        enemy.die(); // Mark dead
    }
    
    enemy.applyAnimation();
}
