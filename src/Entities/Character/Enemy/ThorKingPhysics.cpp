#include "Entities/Character/Enemy/ThorKingPhysics.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Character.h"

void ThorKingPhysics::checkObstacles(ThorKing& boss) {
    std::string state = boss.getStateName();
    if (state == "TKRolling" || state == "FlippingDeath") {
        return;
    }
    boss.Enemy::checkObstacles();
}

void ThorKingPhysics::move(ThorKing& boss, float deltaTime) {
    std::string state = boss.getStateName();
    if (state == "TKRolling") {
        float speed = boss.getRollSpeed();
        float dir = boss.getDirection() == MoveDirection::Right ? 1.0f : -1.0f;
        boss.getShape().move(dir * speed * deltaTime, 0.f);
        boss.setPosition(boss.getShape().getPosition());
    } else if (state == "FlippingDeath") {
        boss.Enemy::move(deltaTime); // Usually handles flipping logic via Enemy::move or state
    } else {
        boss.Enemy::move(deltaTime);
    }
}

void ThorKingPhysics::onStomped(ThorKing& boss, Character* attacker) {
    if (boss.getStateName() == "TKStunned") {
        boss.takeDamage(1);
    }
}

void ThorKingPhysics::onSideCollision(ThorKing& boss, Character* attacker) {
    if (boss.getStateName() == "TKStunned") {
        boss.takeDamage(1);
    } else {
        if (attacker) attacker->takeDamage(2);
    }
}

#include "Entities/Character/Enemy/EnemyState.h"

void ThorKingPhysics::takeDamage(ThorKing& boss, int damage) {
    boss.setBossHp(boss.getBossHp() - damage);
    if (boss.getBossHp() <= 0) {
        boss.changeState(std::make_unique<FlippingDeathState>(-300.f));
    } else {
        boss.changeState(std::make_unique<TKPatrolState>(3.0f));
    }
}

void ThorKingPhysics::notifyWallHit(ThorKing& boss) {
    if (boss.getStateName() == "TKRolling") {
        boss.changeState(std::make_unique<TKStunnedState>());
    }
}
