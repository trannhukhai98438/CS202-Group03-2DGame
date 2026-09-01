#include "Entities/Character/Enemy/EnemyStateFactory.h"
#include "Entities/Character/Enemy/EnemyState.h"

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
    return std::make_unique<PatrolState>();
}