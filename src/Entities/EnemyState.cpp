#include "Entities/EnemyState.h"
#include "Entities/Enemy.h"

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
    velocityY += 980.0f * deltaTime; // Gravity acceleration
    sf::Vector2f pos = enemy.getPosition();
    pos.y += velocityY * deltaTime;
    enemy.setPosition(pos);
    
    // Keep sprite in sync with falling position dynamically based on enemy bounds
    sf::Sprite& sp = enemy.getSprite();
    sf::FloatRect bounds = enemy.getBounds();
    sp.setPosition(pos.x + bounds.width / 2.0f, pos.y + bounds.height);

    // Once fallen off screen, remove cleanly
    if (pos.y > 750.0f) {
        enemy.die();
    }
}
