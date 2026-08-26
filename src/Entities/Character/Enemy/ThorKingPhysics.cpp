#include "Entities/Character/Enemy/ThorKingPhysics.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Character.h"
#include "Entities/Character/Hero/Hero.h"
#include <iostream>

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
        float dir = (boss.getDirection() == MoveDirection::Right) ? 1.0f : -1.0f;
        boss.setPosition(sf::Vector2f(boss.getPosition().x + dir * speed * deltaTime, boss.getPosition().y));
    } else if (state == "TKPatrol") {
        boss.Enemy::move(deltaTime);
    }
}

void ThorKingPhysics::onStomped(ThorKing& boss, Character* attacker) {
    if (boss.getStateName() == "TKStunned") {
        boss.takeDamage(1);
        Hero* hero = dynamic_cast<Hero*>(attacker);
        if (hero) {
            float bounceDir = (hero->getPosition().x < boss.getPosition().x) ? -250.f : 250.f;
            hero->setVelocity(bounceDir, -550.f); // Safe high bounce away from boss
            hero->setInvincible(1.8f);           // 1.8s invulnerability so Mario can land safely
        }
    } else if (boss.getStateName() == "TKRoar") {
        Hero* hero = dynamic_cast<Hero*>(attacker);
        if (hero) {
            float bounceDir = (hero->getPosition().x < boss.getPosition().x) ? -200.f : 200.f;
            hero->setVelocity(bounceDir, -350.f);
            hero->setInvincible(1.0f);
        }
    } else {
        // Armored back with sharp spikes hurts Mario!
        if (attacker) {
            attacker->takeDamage(1);
        }
    }
}

void ThorKingPhysics::onSideCollision(ThorKing& boss, Character* attacker) {
    if (boss.getStateName() == "TKStunned") {
        boss.takeDamage(1);
        Hero* hero = dynamic_cast<Hero*>(attacker);
        if (hero) {
            float bounceDir = (hero->getPosition().x < boss.getPosition().x) ? -250.f : 250.f;
            hero->setVelocity(bounceDir, -550.f);
            hero->setInvincible(1.8f);
        }
    } else if (boss.getStateName() == "TKRoar") {
        // During roar/transformation, boss does NOT damage Mario!
        Hero* hero = dynamic_cast<Hero*>(attacker);
        if (hero) {
            float pushDir = (hero->getPosition().x < boss.getPosition().x) ? -200.f : 200.f;
            hero->setVelocity(pushDir, -200.f);
        }
    } else {
        if (attacker) {
            attacker->takeDamage(1);
        }
    }
}

#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Enemy/ThorKingState.h"

void ThorKingPhysics::takeDamage(ThorKing& boss, int damage) {
    boss.setBossHp(boss.getBossHp() - damage);
    if (boss.getBossHp() <= 0) {
        boss.setIsAlive(false);
        boss.changeState(std::make_unique<FlippingDeathState>(-300.f));
    } else {
        boss.changeState(std::make_unique<TKRoarState>(0.8f));
    }
}

void ThorKingPhysics::notifyWallHit(ThorKing& boss) {
    if (boss.getStateName() == "TKRolling") {
        if (boss.getPhase() == 3) {
            boss.incrementWallBounceCount();
            if (boss.getWallBounceCount() < 2) {
                // In Phase 3: Bounce off the first wall and roll back furiously!
                boss.flipDirection();
                return;
            }
        }
        boss.resetWallBounceCount();
        boss.changeState(std::make_unique<TKStunnedState>(boss.getStunDuration()));
    }
}
