#include "Entities/Character/Enemy/GoombaPhysics.h"
#include "Entities/Character/Enemy/Goomba.h"
#include "Entities/Character/Enemy/EnemyState.h"

void GoombaPhysics::checkObstacles(Goomba& goomba) {
    if (goomba.getIsSquished() || goomba.getStateName() == "FlippingDeath") return;
    goomba.Enemy::checkObstacles();
}

void GoombaPhysics::move(Goomba& goomba, float deltaTime) {
    if (goomba.getIsSquished() || goomba.getStateName() == "FlippingDeath") return;
    goomba.Enemy::move(deltaTime);
}

void GoombaPhysics::takeDamage(Goomba& goomba, int damage) {
    (void)damage;
    if (!goomba.getIsSquished()) {
        goomba.getShape().setSize(sf::Vector2f(64.0f, 32.0f));
        sf::Vector2f pos = goomba.getPosition();
        goomba.setPosition(sf::Vector2f(pos.x, pos.y + 32.0f));
        goomba.changeState(std::make_unique<SquishedState>(0.6f, 0.3f));
    }
}

void GoombaPhysics::onStomped(Goomba& goomba, BaseEntity* attacker) {
    (void)attacker;
    takeDamage(goomba, 1);
}
