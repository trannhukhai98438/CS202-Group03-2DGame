#pragma once
#include "Entities/Enemy.h"
#include "Entities/Projectile.h"
#include "Utilities/Animator.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

class ThrowState : public EnemyState {
private:
    float throwTimer;
    float stateDuration;
    bool hasThrown;
public:
    ThrowState(float duration = 1.0f) : throwTimer(0.0f), stateDuration(duration), hasThrown(false) {}
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    void onExit(Enemy& enemy) override;
    std::string getName() const override { return "Throw"; }
    bool hasThrownFlag() const { return hasThrown; }
};

class Witch : public Enemy {
private:
    float attackCooldown;
    std::function<void(std::unique_ptr<Projectile>)> spawnProjectileCallback;

public:
    Witch(float startX, float startY, float patrolRange, std::function<void(std::unique_ptr<Projectile>)> spawnCallback);
    ~Witch() override = default;

    int getDamageOnTouch() const override;
    void onStomped(BaseEntity* attacker) override;
    void takeDamage(int damage) override;
    
    void update(float deltaTime) override;
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;

    void throwPotion();
};
