#ifndef ENEMY_FACTORY_H
#define ENEMY_FACTORY_H

#include "Enemy.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Projectile.h"
#include <memory>
#include <string>
#include <functional>

enum class EnemyType {
    Goomba,
    Koopa,
    Witch
};

class EnemyFactory {
public:
    EnemyFactory() = default;
    virtual ~EnemyFactory() = default;

    static std::unique_ptr<Enemy> createEnemy(EnemyType type, float x, float y, float patrolRange = 100.0f, std::function<void(std::unique_ptr<Projectile>)> spawnCallback = nullptr);
    static std::unique_ptr<Enemy> createEnemyFromString(const std::string& typeStr, float x, float y, float patrolRange = 100.0f);
};

class BaseEnemyFactory {
public:
    virtual ~BaseEnemyFactory() = default;
    virtual std::unique_ptr<Enemy> create(float x, float y, float patrolRange = 100.0f) const = 0;
};

class ConcreteGoombaFactory : public BaseEnemyFactory {
public:
    std::unique_ptr<Enemy> create(float x, float y, float patrolRange = 100.0f) const override {
        return std::make_unique<Goomba>(x, y, patrolRange);
    }
};

class ConcreteKoopaFactory : public BaseEnemyFactory {
public:
    std::unique_ptr<Enemy> create(float x, float y, float patrolRange = 120.0f) const override {
        return std::make_unique<Koopa>(x, y, patrolRange);
    }
};

#endif
