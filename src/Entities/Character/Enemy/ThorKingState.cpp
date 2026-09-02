#include "Entities/Character/Enemy/ThorKingState.h"
#include "Entities/Character/Enemy/ThorKing.h"

// ==================== TKPatrolState ====================
TKPatrolState::TKPatrolState(float fireCooldown) : m_fireCooldown(fireCooldown) {}

void TKPatrolState::onEnter(Enemy& enemy) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    float speed = boss->getSpeed();
    if (speed <= 0.0f) speed = 150.0f;

    float dirSign = (boss->getDirection() == MoveDirection::Right) ? 1.0f : -1.0f;
    boss->setVelocity({ speed * dirSign, boss->getVelocity().y });
}

void TKPatrolState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    if (std::abs(boss->getVelocity().x) < 1.0f) {
        float speed = boss->getSpeed();
        if (speed <= 0.0f) speed = 150.0f;
        float dirSign = (boss->getDirection() == MoveDirection::Right) ? 1.0f : -1.0f;
        boss->setVelocity({ speed * dirSign, boss->getVelocity().y });
    }

    m_fireCooldown -= deltaTime;
    if (m_fireCooldown <= 0.f) {
        boss->changeState(std::make_unique<TKFireAttackState>());
        return;
    }

    boss->move(deltaTime);
    boss->applyAnimation();
}

// ==================== TKCrouchState ====================
TKCrouchState::TKCrouchState() : m_timer(0.5f) {}

void TKCrouchState::onEnter(Enemy& enemy) {
}

void TKCrouchState::update(Enemy& enemy, float deltaTime) {
    m_timer -= deltaTime;
    if (m_timer <= 0.f) {
        enemy.changeState(std::make_unique<TKRollingState>());
    }
    enemy.applyAnimation();
}

// ==================== TKRollingState ====================
void TKRollingState::onEnter(Enemy& enemy) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    float rollSpeed = boss->getRollSpeed();
    float dirSign = (boss->getDirection() == MoveDirection::Right) ? 1.0f : -1.0f;
    boss->setVelocity({ rollSpeed * dirSign, boss->getVelocity().y });
}

void TKRollingState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    boss->move(deltaTime);
    boss->applyAnimation();
}

// ==================== TKStunnedState ====================
TKStunnedState::TKStunnedState(float duration) : m_timer(duration) {}

void TKStunnedState::onEnter(Enemy& enemy) {}

void TKStunnedState::update(Enemy& enemy, float deltaTime) {
    m_timer -= deltaTime;
    if (m_timer <= 0.f) {
        enemy.changeState(std::make_unique<TKPatrolState>(3.0f));
    }
    enemy.applyAnimation();
}

// ==================== TKFireAttackState ====================
TKFireAttackState::TKFireAttackState() : m_timer(0.90f), m_shotsFired(0), m_totalShots(1), m_nextShotTime(0.5f) {}

void TKFireAttackState::onEnter(Enemy& enemy) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (boss) {
        m_totalShots = boss->getFireBurstCount();
        if (boss->getPhase() == 3) {
            boss->incrementShotSeq(); // Increment shot sequence at the start of the attack
            // Phase 3: Sync duration with 10 animation frames * 0.12s = 1.20s total
            // Plays once: 0.0s-0.36s windup -> 0.36s-1.08s pulsating fire columns -> 1.08s-1.20s cool down
            m_timer = 1.20f;
            m_nextShotTime = 0.84f; // Shoots at exactly t=0.36s elapsed (start of Frame 3)
            m_totalShots = 1;       // Shoot exactly 1 skill per attack state to prevent overlap
        } else {
            m_timer = 0.6f + static_cast<float>(m_totalShots) * 0.35f;
            m_nextShotTime = m_timer - 0.45f;
        }
    }
}

void TKFireAttackState::update(Enemy& enemy, float deltaTime) {
    ThorKing* boss = dynamic_cast<ThorKing*>(&enemy);
    if (!boss) return;

    m_timer -= deltaTime;
    if (m_shotsFired < m_totalShots && m_timer <= m_nextShotTime) {
        boss->spawnFireProjectile();
        m_shotsFired++;
        m_nextShotTime -= 0.35f;
    }

    if (m_timer <= 0.f) {
        boss->incrementFireCount();
        if (boss->getFireCount() >= 1) {
            boss->resetFireCount();
            boss->changeState(std::make_unique<TKCrouchState>());
        } else {
            boss->changeState(std::make_unique<TKPatrolState>(2.0f));
        }
        return;
    }
    enemy.applyAnimation();
}

// ==================== TKRoarState ====================
TKRoarState::TKRoarState(float duration) : m_timer(duration) {}

void TKRoarState::onEnter(Enemy& enemy) {}

void TKRoarState::update(Enemy& enemy, float deltaTime) {
    m_timer -= deltaTime;
    if (m_timer <= 0.f) {
        enemy.changeState(std::make_unique<TKPatrolState>(1.5f));
        return;
    }
    enemy.applyAnimation();
}