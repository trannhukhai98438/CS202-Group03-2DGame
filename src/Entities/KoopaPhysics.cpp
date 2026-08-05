#include "Entities/KoopaPhysics.h"
#include "Entities/Koopa.h"
#include "Entities/EnemyState.h"
#include <cmath>

void KoopaPhysics::checkObstacles(Koopa& koopa) {
    if (koopa.getStateName() == "Shell" || koopa.getStateName() == "FlippingDeath") return;
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
        float newY = pos.y + 18.0f; // 50.0f - 32.0f = 18.0f to keep bottom at ground Y=600
        koopa.getShape().setSize(sf::Vector2f(32.0f, 32.0f));
        koopa.setPosition(sf::Vector2f(pos.x, newY));
    }
    koopa.changeState(std::make_unique<ShellState>());
}

void KoopaPhysics::onStomped(Koopa& koopa, BaseEntity* attacker) {
    (void)attacker;
    if (!koopa.getIsAlive()) return;

    std::string state = koopa.getStateName();
    
    if (state == "Patrol") {
        becomeStaticShell(koopa);
    } else if (state == "Shell" || state == "SpinningShell") {
        takeDamage(koopa, 1);
    }
}

void KoopaPhysics::onSideCollision(Koopa& koopa, BaseEntity* attacker) {
    if (!koopa.getIsAlive()) return;

    std::string state = koopa.getStateName();
    
    if (state == "Shell") {
        MoveDirection kickDir = (attacker && attacker->getPosition().x < koopa.getPosition().x) ? MoveDirection::Right : MoveDirection::Left;
        kickShell(koopa, static_cast<int>(kickDir));
    } else {
        if (attacker) {
            attacker->takeDamage(koopa.getDamageOnTouch());
        }
    }
}

void KoopaPhysics::kickShell(Koopa& koopa, int dir) {
    koopa.setDirection(static_cast<MoveDirection>(dir));
    koopa.changeState(std::make_unique<SpinningShellState>());
}
