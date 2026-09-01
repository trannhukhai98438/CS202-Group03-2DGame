#include "Managers/SaveManager.hpp"

#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Enemy/EnemyStateFactory.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Item/Item.h"
#include "Entities/Item/ItemFactory.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Projectile/ProjectileFactory.h"
#include "Core/Gameplay/GameWorld.h"

#include <set>
#include <utility>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool SaveManager::saveToFile(const std::string& filePath,
                           const std::string& mapPath,
                           const std::string& tilesetPath,
                           const GameWorld& world,
                           const HUDManager& hud) 
{
    if (!world.hero()) {
        std::cerr << "[SaveManager] Failed to save: Hero instance is null!\n";
        return false;
    }

    std::cout << "[SaveManager] Starting game save process to '" << filePath << "'...\n";

    // 1. Level & Camera Metadata
    m_saveData.mapPath = mapPath;
    m_saveData.tilesetPath = tilesetPath;

    // 2. Hero Attributes
    const Hero* hero = world.hero();
    m_saveData.hero.posX = hero->getPosition().x;
    m_saveData.hero.posY = hero->getPosition().y;
    m_saveData.hero.hp = hero->getHp();
    m_saveData.hero.coin = hero->getCoin();
    m_saveData.hero.heroType = hero->getHeroType();
    m_saveData.hero.formName = hero->getFormName();
    m_saveData.hero.invincibleTimer = hero->getInvincibleTimer();
    m_saveData.hero.isStarman = hero->getIsStarman();

    // 3. Enemies
    m_saveData.aliveEnemies.clear();
    for (const auto& enemy : world.enemies()) {
        if (enemy && enemy->getIsAlive()) {
            SaveData::EnemyData eData;
            eData.type = enemy->getCharacterType();
            eData.posX = enemy->getPosition().x;
            eData.posY = enemy->getPosition().y;
            eData.velX = enemy->getVelocity().x;
            eData.velY = enemy->getVelocity().y;
            eData.direction = static_cast<int>(enemy->getDirection());
            eData.health = enemy->getHealth();
            eData.isGrounded = enemy->getGrounded();
            eData.aiState = enemy->getStateName();
            eData.stateTimer = enemy->getStateTimer();
            m_saveData.aliveEnemies.push_back(eData);
        }
    }

    // 4. Hit & Destroyed blocks (Quản lý trực tiếp từ danh sách blocks trong World)
    m_saveData.hitBlocks.clear();
    m_saveData.destroyedBlocks.clear();

    const int tileW = world.levelManager().getTileWidth() > 0 ? world.levelManager().getTileWidth() : 32;
    const int tileH = world.levelManager().getTileHeight() > 0 ? world.levelManager().getTileHeight() : 32;

    for (const auto& coord : world.getDestroyedBlocks()) {
        SaveData::BlockPosition bPos;
        bPos.posX = static_cast<float>(coord.first * tileW);
        bPos.posY = static_cast<float>(coord.second * tileH);
        m_saveData.destroyedBlocks.push_back(bPos);
    }

    for (const auto& block : world.blocks()) {
        if (!block) continue;

        sf::Vector2f pos = block->getPosition();
        int tx = static_cast<int>(std::round(pos.x / tileW));
        int ty = static_cast<int>(std::round(pos.y / tileH));

        if (block->getIsActive() && block->getIsHit()) {
            m_saveData.hitBlocks.push_back({ static_cast<float>(tx * tileW), static_cast<float>(ty * tileH) });
        }
    }

    // 5. Active Items
    m_saveData.activeItems.clear();
    for (const auto& item : world.items()) {
        if (item && !item->isCollected()) {
            SaveData::ItemData iData;
            iData.type = item->getItemType();
            iData.posX = item->getPosition().x;
            iData.posY = item->getPosition().y;
            m_saveData.activeItems.push_back(iData);
        }
    }

    // 6. Active Projectiles
    m_saveData.activeProjectiles.clear();
    for (const auto& proj : world.projectiles()) {
        if (proj && proj->getIsAlive()) {
            SaveData::ProjectileData pData;
            pData.type = proj->getProjectileType();
            pData.posX = proj->getPosition().x;
            pData.posY = proj->getPosition().y;
            pData.velX = proj->getVelocity().x;
            pData.velY = proj->getVelocity().y;
            pData.faction = proj->getFaction();
            m_saveData.activeProjectiles.push_back(pData);
        }
    }

    // 7. HUD Data
    m_saveData.hud.score = hud.getScore();
    m_saveData.hud.coins = hud.getCoins();
    m_saveData.hud.lives = hud.getLives();
    m_saveData.hud.remainingTime = hud.getRemainingTime();

    // Serialize to JSON
    json j;
    j["mapPath"] = m_saveData.mapPath;
    j["tilesetPath"] = m_saveData.tilesetPath;

    j["hero"] = {
        {"posX", m_saveData.hero.posX},
        {"posY", m_saveData.hero.posY},
        {"hp", m_saveData.hero.hp},
        {"coin", m_saveData.hero.coin},
        {"heroType", m_saveData.hero.heroType},
        {"formName", m_saveData.hero.formName},
        {"invincibleTimer", m_saveData.hero.invincibleTimer},
        {"isStarman", m_saveData.hero.isStarman}
    };

    j["aliveEnemies"] = json::array();
    for (const auto& enemy : m_saveData.aliveEnemies) {
        j["aliveEnemies"].push_back({
            {"type", enemy.type},
            {"posX", enemy.posX},
            {"posY", enemy.posY},
            {"velX", enemy.velX},
            {"velY", enemy.velY},
            {"direction", enemy.direction},
            {"health", enemy.health},
            {"isGrounded", enemy.isGrounded},
            {"aiState", enemy.aiState},
            {"stateTimer", enemy.stateTimer}
        });
    }

    j["hitBlocks"] = json::array();
    for (const auto& b : m_saveData.hitBlocks) {
        j["hitBlocks"].push_back({{"posX", b.posX}, {"posY", b.posY}});
    }

    j["destroyedBlocks"] = json::array();
    for (const auto& b : m_saveData.destroyedBlocks) {
        j["destroyedBlocks"].push_back({{"posX", b.posX}, {"posY", b.posY}});
    }

    j["activeItems"] = json::array();
    for (const auto& item : m_saveData.activeItems) {
        j["activeItems"].push_back({
            {"type", item.type},
            {"posX", item.posX},
            {"posY", item.posY}
        });
    }

    j["activeProjectiles"] = json::array();
    for (const auto& proj : m_saveData.activeProjectiles) {
        j["activeProjectiles"].push_back({
            {"type", proj.type},
            {"posX", proj.posX},
            {"posY", proj.posY},
            {"velX", proj.velX},
            {"velY", proj.velY},
            {"faction", static_cast<int>(proj.faction)}
        });
    }

    j["hud"] = {
        {"score", m_saveData.hud.score},
        {"coins", m_saveData.hud.coins},
        {"lives", m_saveData.hud.lives},
        {"remainingTime", m_saveData.hud.remainingTime}
    };

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open file for writing: " << filePath << "\n";
        return false;
    }
    outFile << j.dump(4);
    
    std::cout << "[SaveManager] Game saved successfully to '" << filePath << "'! "
              << "(" << m_saveData.aliveEnemies.size() << " enemies, "
              << m_saveData.activeItems.size() << " active items, "
              << m_saveData.activeProjectiles.size() << " projectiles, "
              << m_saveData.hitBlocks.size() << " hit blocks, "
              << m_saveData.destroyedBlocks.size() << " destroyed blocks saved)\n";
    return true;
}

bool SaveManager::loadFromFile(const std::string& filePath, GameWorld& world) {
    std::cout << "[SaveManager] Loading save data from file '" << filePath << "'...\n";
    
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open save file: " << filePath << "\n";
        return false;
    }

    json j;
    try { 
        inFile >> j; 
    } catch (const std::exception& e) { 
        std::cerr << "[SaveManager] Error parsing JSON file: " << e.what() << "\n";
        return false; 
    }

    m_saveData.mapPath = j.value("mapPath", "");
    m_saveData.tilesetPath = j.value("tilesetPath", "");

    if (j.contains("hero")) {
        auto h = j["hero"];
        m_saveData.hero.posX = h.value("posX", 0.0f);
        m_saveData.hero.posY = h.value("posY", 0.0f);
        m_saveData.hero.hp = h.value("hp", 1);
        m_saveData.hero.coin = h.value("coin", 0);
        m_saveData.hero.heroType = h.value("heroType", "Mario");
        m_saveData.hero.formName = h.value("formName", "SmallForm");
        m_saveData.hero.invincibleTimer = h.value("invincibleTimer", 0.0f);
        m_saveData.hero.isStarman = h.value("isStarman", false);
    }

    m_saveData.aliveEnemies.clear();
    if (j.contains("aliveEnemies") && j["aliveEnemies"].is_array()) {
        for (const auto& item : j["aliveEnemies"]) {
            SaveData::EnemyData eData;
            eData.type = item.value("type", "Goomba");
            eData.posX = item.value("posX", 0.0f);
            eData.posY = item.value("posY", 0.0f);
            eData.velX = item.value("velX", 0.0f);
            eData.velY = item.value("velY", 0.0f);
            eData.direction = item.value("direction", -1);
            eData.health = item.value("health", 1);
            eData.isGrounded = item.value("isGrounded", true);
            eData.aiState = item.value("aiState", "Patrol");
            eData.stateTimer = item.value("stateTimer", -1.0f);
            m_saveData.aliveEnemies.push_back(eData);
        }
    }

    m_saveData.hitBlocks.clear();
    if (j.contains("hitBlocks") && j["hitBlocks"].is_array()) {
        for (const auto& item : j["hitBlocks"]) {
            m_saveData.hitBlocks.push_back({item.value("posX", 0.0f), item.value("posY", 0.0f)});
        }
    }

    m_saveData.destroyedBlocks.clear();
    if (j.contains("destroyedBlocks") && j["destroyedBlocks"].is_array()) {
        for (const auto& item : j["destroyedBlocks"]) {
            m_saveData.destroyedBlocks.push_back({item.value("posX", 0.0f), item.value("posY", 0.0f)});
        }
    }

    m_saveData.activeItems.clear();
    if (j.contains("activeItems") && j["activeItems"].is_array()) {
        for (const auto& item : j["activeItems"]) {
            SaveData::ItemData iData;
            iData.type = item.value("type", "");
            iData.posX = item.value("posX", 0.0f);
            iData.posY = item.value("posY", 0.0f);
            m_saveData.activeItems.push_back(iData);
        }
    }

    m_saveData.activeProjectiles.clear();
    if (j.contains("activeProjectiles") && j["activeProjectiles"].is_array()) {
        for (const auto& item : j["activeProjectiles"]) {
            SaveData::ProjectileData pData;
            pData.type = item.value("type", "");
            pData.posX = item.value("posX", 0.0f);
            pData.posY = item.value("posY", 0.0f);
            pData.velX = item.value("velX", 0.0f);
            pData.velY = item.value("velY", 0.0f);
            int factionInt = item.value("faction", 0);
            pData.faction = static_cast<ProjectileFaction>(factionInt);
            m_saveData.activeProjectiles.push_back(pData);
        }
    }

    if (j.contains("hud")) {
        auto h = j["hud"];
        m_saveData.hud.score = h.value("score", 0);
        m_saveData.hud.coins = h.value("coins", 0);
        m_saveData.hud.lives = h.value("lives", 3);
        m_saveData.hud.remainingTime = h.value("remainingTime", 300.0f);
    }

    std::cout << "[SaveManager] Save data loaded into memory successfully!\n";
    return true;
}

bool SaveManager::applySaveToWorld(GameWorld& world, HUDManager& hud) const {
    std::cout << "[SaveManager] Applying saved data to GameWorld, HUD, and Camera...\n";

    // 1. HUD Restoration
    hud.restoreProgress(m_saveData.hud.score, m_saveData.hud.coins, m_saveData.hud.lives);
    hud.resetTimer(m_saveData.hud.remainingTime);

    // 2. Hero Restoration
    if (auto* hero = world.hero()) {
        HeroType type = HeroType::Mario;
        if (m_saveData.hero.heroType == "Luigi") type = HeroType::Luigi;
        else if (m_saveData.hero.heroType == "Flash") type = HeroType::Flash;

        auto spawnCallback = [&world](std::unique_ptr<Projectile> projectile) {
            world.addProjectile(std::move(projectile));
        };

        auto newHero = HeroFactory::createHero(type, m_saveData.hero.posX, m_saveData.hero.posY, spawnCallback);

        if (newHero) {
            newHero->setForm(m_saveData.hero.formName);
            newHero->setCoin(m_saveData.hero.coin);
            newHero->setHp(m_saveData.hero.hp);
            newHero->setInvincible(m_saveData.hero.invincibleTimer, m_saveData.hero.isStarman);

            world.setHero(std::move(newHero));

            std::cout << "[SaveManager] Hero restored at (" << m_saveData.hero.posX << ", " << m_saveData.hero.posY 
                      << ") | Type: " << m_saveData.hero.heroType
                      << " | Form: " << m_saveData.hero.formName << "\n";
        }
    } else {
        std::cerr << "[SaveManager] Warning: Hero instance in world is null during restoration!\n";
    }

    // 3. Block Restoration (Hit & Destroyed)
    int hitBlockCount = 0;
    int destroyedBlockCount = 0;

    const int tileW = world.levelManager().getTileWidth() > 0 ? world.levelManager().getTileWidth() : 32;
    const int tileH = world.levelManager().getTileHeight() > 0 ? world.levelManager().getTileHeight() : 32;

    std::set<std::pair<int, int>> destroyedSet;
    for (const auto& destData : m_saveData.destroyedBlocks) {
        int tx = static_cast<int>(std::round(destData.posX / tileW));
        int ty = static_cast<int>(std::round(destData.posY / tileH));
        destroyedSet.insert({tx, ty});
        
        // Ép xóa hình ảnh Tile trực tiếp trên Layer Interactive của Map
        world.levelManager().setTileID("Interactive", tx, ty, 0);
    }

    std::set<std::pair<int, int>> hitSet;
    for (const auto& hitData : m_saveData.hitBlocks) {
        int tx = static_cast<int>(std::round(hitData.posX / tileW));
        int ty = static_cast<int>(std::round(hitData.posY / tileH));
        hitSet.insert({tx, ty});
    }

    for (auto& block : world.blocks()) {
        if (!block) continue;

        sf::Vector2f pos = block->getPosition();
        int bTileX = static_cast<int>(std::round(pos.x / tileW));
        int bTileY = static_cast<int>(std::round(pos.y / tileH));

        if (hitSet.count({bTileX, bTileY}) > 0) {
            block->setIsHit(true);
            hitBlockCount++;
        }

        if (destroyedSet.count({bTileX, bTileY}) > 0) {
            block->setIsActive(false);
            destroyedBlockCount++;
        }
    }

    // Xóa thực thể đã inactive ra khỏi vector world.blocks()
    world.removeInactiveEntities();

    std::cout << "[SaveManager] Restored " << hitBlockCount << " hit blocks, removed " 
              << destroyedBlockCount << " destroyed blocks.\n";

    // 4. Enemy Restoration
    world.enemies().clear();
    auto spawnCallback = [&world](std::unique_ptr<Projectile> projectile) {
        world.addProjectile(std::move(projectile));
    };

    for (const auto& eData : m_saveData.aliveEnemies) {
        EnemyType eType = EnemyType::Goomba;
        float speed = 150.f;

        if (eData.type == "Koopa" || eData.type == "koopa") {
            eType = EnemyType::Koopa;
            speed = 200.f;
        } else if (eData.type == "Witch" || eData.type == "witch") {
            eType = EnemyType::Witch;
            speed = 150.f;
        }

        auto enemy = EnemyFactory::createEnemy(eType, eData.posX, eData.posY, speed, spawnCallback);
        if (enemy) {
            MoveDirection dir = (eData.direction >= 0) ? MoveDirection::Right : MoveDirection::Left;
            enemy->setDirection(dir);
            enemy->setHealth(eData.health);
            enemy->setPosition({eData.posX, eData.posY});

            enemy->setVelocity({eData.velX, eData.velY});
            enemy->setGrounded(eData.isGrounded);

            auto restoredState = EnemyStateFactory::createStateFromString(eData.aiState, eData.stateTimer);
            if (restoredState) {
                enemy->changeState(std::move(restoredState));
            }

            world.addEnemy(std::move(enemy));
        }
    }
    std::cout << "[SaveManager] Spawned " << m_saveData.aliveEnemies.size() << " enemies from save data.\n";

    // 5. Active Items Restoration
    world.items().clear();
    for (const auto& iData : m_saveData.activeItems) {
        auto item = ItemFactory::createItem(iData.type, iData.posX, iData.posY);
        if (item) {
            world.addItem(std::move(item));
        }
    }
    std::cout << "[SaveManager] Restored " << m_saveData.activeItems.size() << " active items from save data.\n";

    // 6. Active Projectiles Restoration
    world.projectiles().clear();
    for (const auto& pData : m_saveData.activeProjectiles) {
        auto proj = ProjectileFactory::createProjectile(
            pData.type, pData.posX, pData.posY, pData.velX, pData.velY, pData.faction
        );
        if (proj) {
            world.addProjectile(std::move(proj));
        }
    }
    std::cout << "[SaveManager] Restored " << m_saveData.activeProjectiles.size() << " active projectiles from save data.\n";

    std::cout << "[SaveManager] World restoration completed successfully!\n";
    return true;
}