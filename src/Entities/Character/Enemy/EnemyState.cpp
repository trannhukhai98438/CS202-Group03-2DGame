#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Enemy/Enemy.h"

void PatrolState::update(Enemy& enemy, float deltaTime) {
    enemy.checkObstacles();
    enemy.move(deltaTime);
    enemy.applyAnimation();
}

SquishedState::SquishedState(float duration, float fadeTime, float initialTimer) 
    : timer(initialTimer > 0.0f ? initialTimer : duration), fadeDuration(fadeTime) {}

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

FlippingDeathState::FlippingDeathState(float initialVelY)
    : velocityY(initialVelY), alpha(255.0f) {}

void FlippingDeathState::onEnter(Enemy& enemy) {
    sf::Vector2f scale = enemy.getSprite().getScale();
    enemy.getSprite().setScale(scale.x, -std::abs(scale.y));
    alpha = static_cast<float>(enemy.getSprite().getColor().a);
}

void FlippingDeathState::update(Enemy& enemy, float deltaTime) {
    velocityY += 450.0f * deltaTime; // Soft, smooth gravity acceleration
    sf::Vector2f pos = enemy.getPosition();
    enemy.setPosition(sf::Vector2f(pos.x, pos.y + velocityY * deltaTime));

    // Fade out
    sf::Sprite& sprite = enemy.getSprite();
    sf::Color color = sprite.getColor();
    alpha = std::max(0.0f, alpha - 120.0f * deltaTime);
    color.a = static_cast<sf::Uint8>(alpha);
    sprite.setColor(color);
    if (alpha <= 0.0f) enemy.die();
    
    enemy.applyAnimation();
}
