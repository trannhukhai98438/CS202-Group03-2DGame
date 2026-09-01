#ifndef ENEMY_STATE_FACTORY_H
#define ENEMY_STATE_FACTORY_H

#include <memory>
#include <string>

class EnemyState; // Forward declaration

class EnemyStateFactory {
public:
    static std::unique_ptr<EnemyState> createStateFromString(const std::string& name, float timer = -1.0f);
};

#endif