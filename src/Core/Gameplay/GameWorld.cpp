#include "Gameplay/GameWorld.h"

#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Goal/LevelGoal.h"
#include "Entities/Item/Item.h"

#include <algorithm>
#include <cmath>

GameWorld::GameWorld() {
    m_blockThemePalette.load();
}
GameWorld::~GameWorld() = default;

LevelManager& GameWorld::levelManager() {
    return m_levelManager;
}

const LevelManager& GameWorld::levelManager() const {
    return m_levelManager;
}

BlockThemePalette& GameWorld::blockThemePalette() {
    return m_blockThemePalette;
}

const BlockThemePalette& GameWorld::blockThemePalette() const {
    return m_blockThemePalette;
}

void GameWorld::setSoundManager(SoundManager* soundManager) {
    m_soundManager = soundManager;

    if (m_hero) {
        m_hero->playSFXCallback = [this](const std::string& name) {
            if (m_soundManager) {
                m_soundManager->playSFX(name);
            }
        };
    }
}

Hero* GameWorld::hero() {
    return m_hero.get();
}

const Hero* GameWorld::hero() const {
    return m_hero.get();
}

void GameWorld::setHero(std::unique_ptr<Hero> hero) {
    m_hero = std::move(hero);

    if (m_hero) {
        m_hero->playSFXCallback = [this](const std::string& name) {
            if (m_soundManager) {
                m_soundManager->playSFX(name);
            }
        };
    }
}

std::vector<std::unique_ptr<Block>>& GameWorld::blocks() {
    return m_blocks;
}

const std::vector<std::unique_ptr<Block>>& GameWorld::blocks() const {
    return m_blocks;
}

std::vector<std::unique_ptr<Item>>& GameWorld::items() {
    return m_items;
}

const std::vector<std::unique_ptr<Item>>& GameWorld::items() const {
    return m_items;
}

std::vector<std::unique_ptr<Enemy>>& GameWorld::enemies() {
    return m_enemies;
}

const std::vector<std::unique_ptr<Enemy>>& GameWorld::enemies() const {
    return m_enemies;
}

std::vector<std::unique_ptr<Projectile>>& GameWorld::projectiles() {
    return m_projectiles;
}

const std::vector<std::unique_ptr<Projectile>>& GameWorld::projectiles() const {
    return m_projectiles;
}

std::vector<std::unique_ptr<LevelGoal>>& GameWorld::goals() {
    return m_goals;
}

const std::vector<std::unique_ptr<LevelGoal>>& GameWorld::goals() const {
    return m_goals;
}

std::vector<sf::RectangleShape>& GameWorld::mapColliders() {
    return m_mapColliders;
}

const std::vector<sf::RectangleShape>& GameWorld::mapColliders() const {
    return m_mapColliders;
}

void GameWorld::addBlock(std::unique_ptr<Block> block) {
    if (block) m_blocks.push_back(std::move(block));
}

void GameWorld::addItem(std::unique_ptr<Item> item) {
    if (item) m_items.push_back(std::move(item));
}

void GameWorld::addEnemy(std::unique_ptr<Enemy> enemy) {
    if (enemy) m_enemies.push_back(std::move(enemy));
}

void GameWorld::addProjectile(std::unique_ptr<Projectile> projectile) {
    if (projectile) m_projectiles.push_back(std::move(projectile));
}

void GameWorld::addGoal(std::unique_ptr<LevelGoal> goal) {
    if (goal) m_goals.push_back(std::move(goal));
}

void GameWorld::removeInactiveEntities() {
    const int tileW = m_levelManager.getTileWidth() > 0 ? m_levelManager.getTileWidth() : 32;
    const int tileH = m_levelManager.getTileHeight() > 0 ? m_levelManager.getTileHeight() : 32;

    for (const auto& block : m_blocks) {
        if (block && !block->getIsActive()) {
            sf::Vector2f pos = block->getPosition();
            int tx = static_cast<int>(std::round(pos.x / tileW));
            int ty = static_cast<int>(std::round(pos.y / tileH));
            m_destroyedBlocks.insert({tx, ty});
        }
    }

    m_blocks.erase(
        std::remove_if(m_blocks.begin(), m_blocks.end(),
                       [](const std::unique_ptr<Block>& block) {
                           return !block || !block->getIsActive();
                       }),
        m_blocks.end());

    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
                       [](const std::unique_ptr<Item>& item) {
                           return !item || item->isCollected();
                       }),
        m_items.end());

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
                       [](const std::unique_ptr<Enemy>& enemy) {
                           return !enemy || !enemy->getIsAlive();
                       }),
        m_enemies.end());

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const std::unique_ptr<Projectile>& projectile) {
                           return !projectile || !projectile->getIsAlive();
                       }),
        m_projectiles.end());
}

void GameWorld::clear() {
    m_hero.reset();
    m_blocks.clear();
    m_items.clear();
    m_enemies.clear();
    m_projectiles.clear();
    m_goals.clear();
    m_mapColliders.clear();
    m_destroyedBlocks.clear();
}
