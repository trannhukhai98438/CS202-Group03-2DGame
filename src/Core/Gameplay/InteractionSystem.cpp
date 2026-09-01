#include "Gameplay/InteractionSystem.h"

#include "Gameplay/GameWorld.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Goal/LevelGoal.h"
#include "Entities/Item/Item.h"

#include <memory>
#include <unordered_set>

int InteractionSystem::resolve(GameWorld& world) {
    int scoreDelta = 0;
    scoreDelta += resolveHeroItems(world);
    scoreDelta += resolveSpinningShells(world);
    scoreDelta += resolveHeroEnemies(world);
    scoreDelta += resolveProjectiles(world);
    scoreDelta += resolveHeroGoals(world);
    return scoreDelta;
}

int InteractionSystem::resolveHeroItems(GameWorld& world) {
    Hero* hero = world.hero();
    if (!hero || hero->isDead()) return 0;

    for (auto& item : world.items()) {
        if (item->isColliable()) {
            if (m_physics.checkCollision(hero->getHitbox(), item->getHitbox())
                != SideType::None) {
                hero->collectItem(item.get());
                if (SoundManager* sm = world.getSoundManager())
                    sm->playSFX("powerup");
            }
        } else if (hero->getBounds().intersects(item->getBounds())) {
            hero->collectItem(item.get());
            if (SoundManager* sm = world.getSoundManager())
                sm->playSFX("powerup");
        }
    }
    return 0;
}

int InteractionSystem::resolveSpinningShells(GameWorld& world) {
    int scoreDelta = 0;
    auto& enemies = world.enemies();

    for (auto& enemy : enemies) {
        if (!enemy->getIsAlive() || enemy->getStateName() != "SpinningShell") {
            continue;
        }

        for (auto& otherEnemy : enemies) {
            if (!otherEnemy->getIsAlive() || otherEnemy.get() == enemy.get()
                || otherEnemy->getStateName() == "FlippingDeath"
                || otherEnemy->getStateName() == "Squished") {
                continue;
            }
            if (!enemy->getBounds().intersects(otherEnemy->getBounds())) {
                continue;
            }

            otherEnemy->changeState(
                std::make_unique<FlippingDeathState>(-300.0f));
            scoreDelta += 200;

            if (SoundManager* sm = world.getSoundManager()) {
                sm->playSFX("kick");
            }
        }
    }
    return scoreDelta;
}

int InteractionSystem::resolveHeroEnemies(GameWorld& world) {
    Hero* hero = world.hero();
    if (!hero || hero->isDead()) return 0;

    int scoreDelta = 0;
    for (auto& enemy : world.enemies()) {
        // A preceding enemy in this same pass may have killed the Hero.
        // Stop immediately so death cannot trigger more damage responses.
        if (hero->isDead()) break;

        if (!enemy->getIsAlive()
            || enemy->getStateName() == "FlippingDeath"
            || enemy->getStateName() == "Squished"
            || !enemy->getBounds().intersects(hero->getBounds())) {
            continue;
        }

        int scoreEarned = hero->interactWith(enemy.get());
        if (scoreEarned > 0) {
            scoreDelta += scoreEarned;
            if (SoundManager* sm = world.getSoundManager())
                sm->playSFX("stomp");
        }
    }
    return scoreDelta;
}

int InteractionSystem::resolveProjectiles(GameWorld& world) {
    int scoreDelta = 0;
    for (auto& projectile : world.projectiles()) {
        scoreDelta += resolveProjectileTargets(world, *projectile);
    }
    return scoreDelta;
}

int InteractionSystem::resolveProjectileTargets(GameWorld& world,
                                                 Projectile& projectile) {
    if (!projectile.getIsAlive()) return 0;

    Hero* hero = world.hero();
    if (projectile.getFaction() == ProjectileFaction::Enemy) {
        if (hero && !hero->isDead()
            && projectile.getBounds().intersects(hero->getBounds())) {
            projectile.onHitTarget(*hero);
        }
        projectile.onTargetResolutionComplete();
        return 0;
    }

    int scoreDelta = 0;

    // A projectile may expand its target bounds on impact. The second pass
    // catches targets that appeared earlier in the list; the set guarantees
    // one resolution per target in this frame.
    std::unordered_set<Enemy*> resolvedTargets;
    for (int pass = 0; pass < 2 && projectile.getIsAlive(); ++pass) {
        for (auto& enemy : world.enemies()) {
            if (!projectile.getIsAlive()) break;
            if (!enemy->getIsAlive()
                || enemy->getStateName() == "FlippingDeath"
                || enemy->getStateName() == "Squished"
                || resolvedTargets.find(enemy.get()) != resolvedTargets.end()) {
                continue;
            }
            if (!projectile.getBounds().intersects(enemy->getBounds())) {
                continue;
            }

            resolvedTargets.insert(enemy.get());
            if (projectile.onHitTarget(*enemy)) {
                scoreDelta += enemy->getScoreValue();
                if (SoundManager* sm = world.getSoundManager())
                    sm->playSFX("kick");
            }
        }
    }
    projectile.onTargetResolutionComplete();
    return scoreDelta;
}

int InteractionSystem::resolveHeroGoals(GameWorld& world) {
    Hero* hero = world.hero();
    if (!hero || hero->isDead()) return 0;

    for (auto& goal : world.goals()) {
        GoalResult result = goal->tryActivate(*hero);
        if (result.activated) {
            if (SoundManager* sm = world.getSoundManager())
                sm->playSFX("flagpole");

            return result.scoreAwarded;
        }
    }
    return 0;
}
