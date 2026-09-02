#include "Entities/Character/Enemy/KoopaPhysics.h"
#include "Entities/Character/Enemy/Koopa.h"
#include "Entities/Character/Enemy/EnemyState.h"
#include <cmath>

void KoopaPhysics::checkObstacles(Koopa& koopa) {
    if (koopa.getStateName() == "Shell" || koopa.getStateName() == "FlippingDeath" || koopa.getStateName() == "SpinningShell") return;
    koopa.Enemy::checkObstacles();
}

void KoopaPhysics::move(Koopa& koopa, float deltaTime) {
    if (koopa.getStateName() == "Shell" || koopa.getStateName() == "FlippingDeath") return;

    if (koopa.getStateName() == "SpinningShell") {
        float moveAmount = static_cast<float>(koopa.getDirection()) * koopa.getShellSpeed() * deltaTime;
        sf::Vector2f pos = koopa.getPosition();
        koopa.setPosition(sf::Vector2f(pos.x + moveAmount, pos.y));
        return;
    }

    koopa.Enemy::move(deltaTime);
}

void KoopaPhysics::takeDamage(Koopa& koopa, int damage) {
    (void)damage;
    if (!koopa.getIsAlive()) return;
    koopa.changeState(std::make_unique<FlippingDeathState>());
}

void KoopaPhysics::becomeStaticShell(Koopa& koopa) {
    if (koopa.getStateName() == "Patrol") {
        sf::Vector2f pos = koopa.getPosition();
        float newY = pos.y + 20.0f; // 48.0f - 28.0f = 20.0f Y translation to align shell bottom to ground
        koopa.getShape().setSize(sf::Vector2f(30.0f, 28.0f));
        koopa.setPosition(sf::Vector2f(pos.x, newY));
    } else if (koopa.getStateName() == "SpinningShell") {
        koopa.setVelocity(0.0f, koopa.getVelocity().y);
    }
    koopa.setShellTimer(5.0f); // Set 5 seconds timer to wake up!
    koopa.setKickCooldown(0.2f); // Cooldown to avoid instant kick on stomp bounce overlap!
    koopa.changeState(std::make_unique<ShellState>());
}

void KoopaPhysics::onStomped(Koopa& koopa, Character* attacker) {
    if (!koopa.getIsAlive()) return;

    std::string state = koopa.getStateName();
    
    if (state == "Patrol") {
        becomeStaticShell(koopa);
    } else if (state == "Shell") {
        // Stomp on stationary shell -> kick it away from attacker!
        float attackerCenterX = attacker ? (attacker->getPosition().x + attacker->getBounds().width / 2.f) : 0.f;
        float koopaCenterX = koopa.getPosition().x + koopa.getBounds().width / 2.f;
        MoveDirection kickDir = (attackerCenterX < koopaCenterX) ? MoveDirection::Right : MoveDirection::Left;
        kickShell(koopa, static_cast<int>(kickDir));
    } else if (state == "SpinningShell") {
        // Stomp on spinning shell -> stop it!
        becomeStaticShell(koopa);
    }
}

void KoopaPhysics::onSideCollision(Koopa& koopa, Character* attacker) {
    if (!koopa.getIsAlive()) return;

    std::string state = koopa.getStateName();
    
    if (state == "Shell") {
        if (koopa.getKickCooldown() > 0.0f) return; // Prevent kick on overlap during bounce cooldown
        float attackerCenterX = attacker ? (attacker->getPosition().x + attacker->getBounds().width / 2.f) : 0.f;
        float koopaCenterX = koopa.getPosition().x + koopa.getBounds().width / 2.f;
        MoveDirection kickDir = (attackerCenterX < koopaCenterX) ? MoveDirection::Right : MoveDirection::Left;
        kickShell(koopa, static_cast<int>(kickDir));
    } else if (state == "SpinningShell") {
        if (koopa.getKickCooldown() <= 0.0f && attacker) {
            attacker->takeDamage(koopa.getDamageOnTouch());
        }
    } else {
        if (attacker) {
            attacker->takeDamage(koopa.getDamageOnTouch());
        }
    }
}

void KoopaPhysics::kickShell(Koopa& koopa, int dir) {
    koopa.setDirection(static_cast<MoveDirection>(dir));
    koopa.changeState(std::make_unique<SpinningShellState>());
    koopa.setKickCooldown(0.3f); // 0.3s of safety frame to not hurt the player who kicked it
}
