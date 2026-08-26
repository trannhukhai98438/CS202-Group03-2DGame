#ifndef THOR_KING_STATE_H
#define THOR_KING_STATE_H

#include "Entities/Character/Enemy/EnemyState.h"

// ---------------------------------------------------------------------------
// TKPatrolState — normal walk patrol, periodically triggers fire attack
// TKPatrolState
class TKPatrolState : public EnemyState {
private:
    float m_fireCooldown;
public:
    explicit TKPatrolState(float fireCooldown = 2.0f);
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKPatrol"; }
};

// ---------------------------------------------------------------------------
// TKCrouchState — boss curling into shell (0.5s transition)
// ---------------------------------------------------------------------------
class TKCrouchState : public EnemyState {
private:
    float m_timer;
public:
    TKCrouchState();
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKCrouch"; }
};

// ---------------------------------------------------------------------------
// TKRollingState — high-speed spiked shell rolling, bounces off walls
// ---------------------------------------------------------------------------
class TKRollingState : public EnemyState {
public:
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKRolling"; }
};

// ---------------------------------------------------------------------------
// TKStunnedState — belly exposed, boss is vulnerable for N seconds
// ---------------------------------------------------------------------------
class TKStunnedState : public EnemyState {
private:
    float m_timer;
    bool m_initialized = false;
public:
    explicit TKStunnedState(float duration = 3.0f);
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKStunned"; }
};

// ---------------------------------------------------------------------------
// TKFireAttackState — boss stands still, fires burst of projectiles
// ---------------------------------------------------------------------------
class TKFireAttackState : public EnemyState {
private:
    float m_timer;
    int   m_shotsFired = 0;
    int   m_totalShots = 1;
    float m_nextShotTime = 0.5f;
public:
    TKFireAttackState();
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKFire"; }
};

// ---------------------------------------------------------------------------
// TKRoarState — boss stands still roaring in rage when damaged/changing phase
// ---------------------------------------------------------------------------
class TKRoarState : public EnemyState {
private:
    float m_timer;
public:
    explicit TKRoarState(float duration = 0.8f);
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "TKRoar"; }
};

#endif // THOR_KING_STATE_H

