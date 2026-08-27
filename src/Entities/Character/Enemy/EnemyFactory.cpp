#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Enemy/Witch.h"
#include <algorithm>

std::unique_ptr<Enemy> EnemyFactory::createEnemy(EnemyType type, float x, float y, float patrolRange, std::function<void(std::unique_ptr<Projectile>)> spawnCallback) {
    switch (type) {
        case EnemyType::Goomba:
            return std::make_unique<Goomba>(x, y, patrolRange);
        case EnemyType::Koopa:
            return std::make_unique<Koopa>(x, y, patrolRange);
        case EnemyType::Witch:
            return std::make_unique<Witch>(x, y, patrolRange, spawnCallback);
        default:
            return nullptr;
    }
}

std::unique_ptr<Enemy> EnemyFactory::createEnemyFromString(const std::string& typeStr, float x, float y, float patrolRange) {
    std::string lowerType = typeStr;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);

    if (lowerType == "goomba") {
        return createEnemy(EnemyType::Goomba, x, y, patrolRange);
    } else if (lowerType == "koopa") {
        return createEnemy(EnemyType::Koopa, x, y, patrolRange);
    } else if (lowerType == "witch") {
        return createEnemy(EnemyType::Witch, x, y, patrolRange);
    }
    return nullptr;
}
