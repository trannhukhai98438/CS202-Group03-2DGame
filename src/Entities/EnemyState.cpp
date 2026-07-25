#include "Entities/EnemyState.h"
#include "Entities/Enemy.h"

void PatrolState::update(Enemy& enemy, float deltaTime) {
    enemy.checkObstacles();
    enemy.move(deltaTime);
    enemy.applyAnimation();
}

SquishedState::SquishedState(float duration) : timer(duration) {}

void SquishedState::onEnter(Enemy& enemy) {
    (void)enemy;
}

void SquishedState::update(Enemy& enemy, float deltaTime) {
    timer -= deltaTime;
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
