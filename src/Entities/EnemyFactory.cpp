#include "Entities/EnemyFactory.h"
#include <algorithm>

std::unique_ptr<Enemy> EnemyFactory::createEnemy(EnemyType type, float x, float y, float patrolRange) {
    switch (type) {
        case EnemyType::Goomba:
            return std::make_unique<Goomba>(x, y, patrolRange);
        case EnemyType::Koopa:
            return std::make_unique<Koopa>(x, y, patrolRange);
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
    }
    return nullptr;
}
