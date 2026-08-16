#include "Gameplay/LevelRuntime.h"

#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Goal/LevelGoal.h"
#include "Entities/Item/Item.h"
#include "Gameplay/LevelBuilder.h"

LevelRuntime::LevelRuntime(const std::string& mapPath,
                           const std::string& tilesetPath,
                           HeroType heroType) {
    LevelBuilder builder;
    m_ready = builder.build(m_world, mapPath, tilesetPath, heroType);
}

int LevelRuntime::update(float deltaTime) {
    if (!m_ready) return 0;

    if (Hero* hero = m_world.hero()) {
        // DeadState owns its death motion, so it must keep receiving updates.
        hero->update(deltaTime);
    }

    for (auto& block : m_world.blocks()) {
        block->update(deltaTime);
    }
    for (auto& item : m_world.items()) {
        item->update(deltaTime);
    }
    for (auto& enemy : m_world.enemies()) {
        enemy->update(deltaTime);
    }
    for (auto& projectile : m_world.projectiles()) {
        projectile->update(deltaTime);
    }

    m_worldPhysics.update(m_world, deltaTime);
    const int scoreDelta = m_interactions.resolve(m_world);

    Hero* hero = m_world.hero();
    if (hero && !hero->isDead() && hero->getPosition().y > 720.0f) {
        hero->takeDamage(1);
    }

    const float worldWidth = getWorldWidth();
    for (auto& projectile : m_world.projectiles()) {
        const sf::FloatRect bounds = projectile->getBounds();
        if (bounds.left + bounds.width < 0.0f
            || bounds.left > worldWidth
            || bounds.top > 900.0f) {
            projectile->die();
        }
    }

    m_world.removeInactiveEntities();
    return scoreDelta;
}

void LevelRuntime::renderWorld(sf::RenderWindow& window) {
    if (!m_ready) return;

    window.draw(m_world.levelManager());

    for (auto& block : m_world.blocks()) {
        block->render(window);
    }
    for (auto& item : m_world.items()) {
        item->render(window);
    }
    if (Hero* hero = m_world.hero()) {
        hero->render(window);
    }
    for (auto& enemy : m_world.enemies()) {
        if (enemy) enemy->render(window);
    }
    for (auto& projectile : m_world.projectiles()) {
        if (projectile) projectile->render(window);
    }
}

Hero* LevelRuntime::getHero() {
    return m_world.hero();
}

const Hero* LevelRuntime::getHero() const {
    return m_world.hero();
}

float LevelRuntime::getWorldWidth() const {
    return static_cast<float>(m_world.levelManager().getMapWidthPixels()) * 2.0f;
}

bool LevelRuntime::hasActivatedGoal() const {
    for (const auto& goal : m_world.goals()) {
        if (goal && goal->isActivated()) return true;
    }
    return false;
}

bool LevelRuntime::isReady() const {
    return m_ready;
}
