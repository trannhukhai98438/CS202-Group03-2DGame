#include "Managers/SaveManager.hpp"

#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Hero/Hero.h"
#include "Gameplay/GameWorld.h"
#include "Gameplay/LevelBuilder.h"
#include "Managers/HUDManager.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool SaveManager::saveToFile(const std::string& filePath,
                            const std::string& mapPath,
                            const std::string& tilesetPath,
                            const Hero* hero,
                            const std::vector<std::unique_ptr<Enemy>>& enemies,
                            const std::vector<std::unique_ptr<Block>>& blocks,
                            const HUDManager& hud) 
{
    if (!hero) return false;

    // 1. Capture Map metadata
    m_saveData.mapPath = mapPath;
    m_saveData.tilesetPath = tilesetPath;

    // 2. Capture Hero state
    m_saveData.hero.posX = hero->getPosition().x;
    m_saveData.hero.posY = hero->getPosition().y;
    m_saveData.hero.hp = hero->getHp();
    m_saveData.hero.coin = hero->getCoin();
    m_saveData.hero.formName = hero->getFormName();

    // 3. Capture active/alive Enemies
    m_saveData.aliveEnemies.clear();
    for (const auto& enemy : enemies) {
        if (enemy && enemy->getIsAlive()) {
            SaveData::EnemyData eData;
            eData.type = enemy->getCharacterType();
            eData.posX = enemy->getPosition().x;
            eData.posY = enemy->getPosition().y;
            eData.direction = static_cast<int>(enemy->getDirection());
            eData.health = enemy->getHealth();
            m_saveData.aliveEnemies.push_back(eData);
        }
    }

    // 4. Capture modified/hit Blocks
    m_saveData.hitBlocks.clear();
    for (const auto& block : blocks) {
        if (block && block->getIsHit()) {
            SaveData::HitBlockData bData;
            bData.posX = block->getBounds().left;
            bData.posY = block->getBounds().top;
            m_saveData.hitBlocks.push_back(bData);
        }
    }

    // 5. Capture HUD state
    m_saveData.hud.score = hud.getScore();
    m_saveData.hud.coins = hud.getCoins();
    m_saveData.hud.lives = hud.getLives();
    m_saveData.hud.remainingTime = hud.getRemainingTime();

    // 6. Serialize internal state to JSON structure
    json j;
    j["mapPath"] = m_saveData.mapPath;
    j["tilesetPath"] = m_saveData.tilesetPath;

    j["hero"] = {
        {"posX", m_saveData.hero.posX},
        {"posY", m_saveData.hero.posY},
        {"hp", m_saveData.hero.hp},
        {"coin", m_saveData.hero.coin},
        {"formName", m_saveData.hero.formName}
    };

    j["aliveEnemies"] = json::array();
    for (const auto& enemy : m_saveData.aliveEnemies) {
        j["aliveEnemies"].push_back({
            {"type", enemy.type},
            {"posX", enemy.posX},
            {"posY", enemy.posY},
            {"direction", enemy.direction},
            {"health", enemy.health}
        });
    }

    j["hitBlocks"] = json::array();
    for (const auto& block : m_saveData.hitBlocks) {
        j["hitBlocks"].push_back({
            {"posX", block.posX},
            {"posY", block.posY}
        });
    }

    j["hud"] = {
        {"score", m_saveData.hud.score},
        {"coins", m_saveData.hud.coins},
        {"lives", m_saveData.hud.lives},
        {"remainingTime", m_saveData.hud.remainingTime}
    };

    // 7. Write formatted JSON string to disk
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }

    outFile << j.dump(4);
    outFile.close();

    std::cout << "[SaveManager] Game state successfully saved to " << filePath << std::endl;
    return true;
}

bool SaveManager::loadFromFile(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open file for reading: " << filePath << std::endl;
        return false;
    }

    json j;
    try {
        inFile >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "[SaveManager] JSON Parse Error: " << e.what() << std::endl;
        return false;
    }
    inFile.close();

    // Deserialize JSON contents into internal SaveData memory cache
    m_saveData.mapPath = j.value("mapPath", "");
    m_saveData.tilesetPath = j.value("tilesetPath", "");

    if (j.contains("hero")) {
        auto h = j["hero"];
        m_saveData.hero.posX = h.value("posX", 0.0f);
        m_saveData.hero.posY = h.value("posY", 0.0f);
        m_saveData.hero.hp = h.value("hp", 1);
        m_saveData.hero.coin = h.value("coin", 0);
        m_saveData.hero.formName = h.value("formName", "SmallForm");
    }

    m_saveData.aliveEnemies.clear();
    if (j.contains("aliveEnemies") && j["aliveEnemies"].is_array()) {
        for (const auto& item : j["aliveEnemies"]) {
            SaveData::EnemyData eData;
            eData.type = item.value("type", "Goomba");
            eData.posX = item.value("posX", 0.0f);
            eData.posY = item.value("posY", 0.0f);
            eData.direction = item.value("direction", -1);
            eData.health = item.value("health", 1);
            m_saveData.aliveEnemies.push_back(eData);
        }
    }

    m_saveData.hitBlocks.clear();
    if (j.contains("hitBlocks") && j["hitBlocks"].is_array()) {
        for (const auto& item : j["hitBlocks"]) {
            SaveData::HitBlockData bData;
            bData.posX = item.value("posX", 0.0f);
            bData.posY = item.value("posY", 0.0f);
            m_saveData.hitBlocks.push_back(bData);
        }
    }

    if (j.contains("hud")) {
        auto h = j["hud"];
        m_saveData.hud.score = h.value("score", 0);
        m_saveData.hud.coins = h.value("coins", 0);
        m_saveData.hud.lives = h.value("lives", 3);
        m_saveData.hud.remainingTime = h.value("remainingTime", 300.0f);
    }

    std::cout << "[SaveManager] Game state successfully loaded from " << filePath << std::endl;
    return true;
}

bool SaveManager::applySaveToWorld(GameWorld& world, HUDManager& hud) const {
    // 1. Restore HUD metrics and timer
    hud.restoreProgress(m_saveData.hud.score, m_saveData.hud.coins, m_saveData.hud.lives);
    hud.resetTimer(m_saveData.hud.remainingTime);

    // 2. Restore Hero position, form, coins, and HP
    if (auto* hero = world.hero()) {
        hero->setPosition(m_saveData.hero.posX, m_saveData.hero.posY);
        hero->setForm(m_saveData.hero.formName);
        hero->setCoin(m_saveData.hero.coin);
        hero->setHp(m_saveData.hero.hp);
    }

    // 3. Restore state for modified/hit Blocks by positional matching
    for (auto& block : world.blocks()) {
        if (!block) continue;
        sf::FloatRect bounds = block->getBounds();
        for (const auto& hitData : m_saveData.hitBlocks) {
            if (std::abs(bounds.left - hitData.posX) < 1.0f && 
                std::abs(bounds.top - hitData.posY) < 1.0f) 
            {
                block->setIsHit(true);
                break;
            }
        }
    }

    // 4. Replace default level enemies with exact saved active enemy instances
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
            world.addEnemy(std::move(enemy));
        }
    }

    std::cout << "[SaveManager] Save data successfully applied to GameWorld." << std::endl;
    return true;
}