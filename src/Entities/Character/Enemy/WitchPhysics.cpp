#include "Entities/Character/Enemy/WitchPhysics.h"
#include "Entities/Character/Enemy/Witch.h"
#include "Entities/Character/Enemy/EnemyState.h"

void WitchPhysics::checkObstacles(Witch& witch) {
    if (witch.getStateName() == "Throw" || witch.getStateName() == "Squished" || witch.getStateName() == "FlippingDeath") return;
    witch.Enemy::checkObstacles();
}

void WitchPhysics::move(Witch& witch, float deltaTime) {
    if (witch.getStateName() == "Throw" || witch.getStateName() == "Squished" || witch.getStateName() == "FlippingDeath") return;
    witch.Enemy::move(deltaTime);
}

void WitchPhysics::takeDamage(Witch& witch, int damage) {
    witch.setHealth(witch.getHealth() - damage);
    if (witch.getHealth() <= 0) {
        witch.changeState(std::make_unique<SquishedState>(0.8f, 0.4f));
    }
}

void WitchPhysics::onStomped(Witch& witch, Character* attacker) {
    (void)attacker;
    takeDamage(witch, 1);
}

void WitchPhysics::updateCooldown(Witch& witch, float deltaTime) {
    if (witch.getStateName() == "Patrol") {
        witch.addAttackCooldown(deltaTime);
        if (witch.getAttackCooldown() >= 3.5f) {
            witch.resetAttackCooldown();
            witch.changeState(std::make_unique<ThrowState>(0.6f));
        }
    }
}
