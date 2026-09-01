#include "Entities/Character/Enemy/EnemyStateFactory.h"
#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Enemy/Witch.h"

std::unique_ptr<EnemyState> EnemyStateFactory::createStateFromString(const std::string& name, float timer) {
    if (name == "Squished") {
        return std::make_unique<SquishedState>(0.6f, 0.3f, timer);
    }
    if (name == "Shell") {
        return std::make_unique<ShellState>();
    }
    if (name == "SpinningShell") {
        return std::make_unique<SpinningShellState>();
    }
    if (name == "FlippingDeath") {
        return std::make_unique<FlippingDeathState>();
    }
    if (name == "Throw") {
        return std::make_unique<ThrowState>(timer > 0.0f ? timer : 1.0f);
    }
    return std::make_unique<PatrolState>();
}