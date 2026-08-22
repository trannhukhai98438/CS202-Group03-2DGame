#include "Entities/Character/Enemy/ThorKingState.h"
#include "Entities/Character/Enemy/ThorKing.h"

// TKPatrolState
TKPatrolState::TKPatrolState(float fireCooldown) : m_fireCooldown(fireCooldown) {}

void TKPatrolState::onEnter(Enemy& enemy) {}

void TKPatrolState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    m_fireCooldown -= deltaTime;
    if (m_fireCooldown <= 0.f) {
        boss->changeState(std::make_unique<TKFireAttackState>());
        return;
    }

    enemy.checkObstacles();
    enemy.move(deltaTime);
    enemy.applyAnimation();
}

// TKCrouchState
TKCrouchState::TKCrouchState() : m_timer(0.5f) {}

void TKCrouchState::onEnter(Enemy& enemy) {
    enemy.getShape().setSize(sf::Vector2f(70.f, 70.f));
    enemy.getShape().move(0.f, 58.f);
    enemy.setPosition(enemy.getShape().getPosition());
}

void TKCrouchState::update(Enemy& enemy, float deltaTime) {
    m_timer -= deltaTime;
    if (m_timer <= 0.f) {
        enemy.changeState(std::make_unique<TKRollingState>());
    }
    enemy.applyAnimation();
}

// TKRollingState
void TKRollingState::onEnter(Enemy& enemy) {}

void TKRollingState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    float speed = boss->getRollSpeed();
    float dirX = (boss->getDirection() == MoveDirection::Right) ? 1.0f : -1.0f;

    boss->getShape().move(dirX * speed * deltaTime, 0.f);
    boss->setPosition(boss->getShape().getPosition());

    boss->applyAnimation();
}

// TKStunnedState
TKStunnedState::TKStunnedState(float duration) : m_timer(duration) {}

void TKStunnedState::onEnter(Enemy& enemy) {
}

void TKStunnedState::update(Enemy& enemy, float deltaTime) {
    if (!m_initialized) {
        enemy.getShape().move(0.f, -58.f);
        enemy.getShape().setSize(sf::Vector2f(96.f, 128.f));
        enemy.setPosition(enemy.getShape().getPosition());
        m_initialized = true;
    }
    m_timer -= deltaTime;
    if (m_timer <= 0.f) {
        enemy.changeState(std::make_unique<TKPatrolState>(3.0f));
    }
    enemy.applyAnimation();
}

// TKFireAttackState
TKFireAttackState::TKFireAttackState() : m_timer(0.90f), m_fired(false) {}

void TKFireAttackState::onEnter(Enemy& enemy) {}

void TKFireAttackState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    m_timer -= deltaTime;
    // Spawn projectile exactly when the roaring mouth-open frame (Frame 2) starts
    // Frame 2 starts at 0.30s (which is 0.90s - 0.60s remaining)
    if (m_timer <= 0.60f && !m_fired) {
        boss->spawnFireProjectile();
        m_fired = true;
    }
    if (m_timer <= 0.f) {
        boss->incrementFireCount();
        if (boss->getFireCount() >= 1) { // Only 1 fire before rolling!
            boss->resetFireCount();
            boss->changeState(std::make_unique<TKCrouchState>());
        } else {
            boss->changeState(std::make_unique<TKPatrolState>(2.0f));
        }
        return;
    }
    enemy.applyAnimation();
}
