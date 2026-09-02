#include "Managers/SaveManager.hpp"

#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Enemy/EnemyStateFactory.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Item/Item.h"
#include "Entities/Item/ItemFactory.h"
#include "Entities/Item/Coin.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Projectile/ProjectileFactory.h"
#include "Core/Gameplay/GameWorld.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <cstdlib>
#include <filesystem>
#include <set>
#include <utility>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
constexpr const char* SAVE_FILE_NAME = "savegame.json";
constexpr const char* GAME_DATA_DIRECTORY = "Custom2DPlatformer";

bool isRegularFile(const std::filesystem::path& path) {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error) && !error;
}

std::string pathForLog(const std::filesystem::path& path) {
    try {
        return path.u8string();
    } catch (const std::exception&) {
        return "<save path>";
    }
}

#ifdef _WIN32
std::filesystem::path getEnvironmentPath(const wchar_t* variableName) {
    const DWORD requiredSize = GetEnvironmentVariableW(
        variableName, nullptr, 0);
    if (requiredSize == 0) return {};

    std::vector<wchar_t> value(requiredSize);
    const DWORD copied = GetEnvironmentVariableW(
        variableName, value.data(), requiredSize);
    if (copied == 0 || copied >= requiredSize) return {};
    return std::filesystem::path(value.data());
}
#endif
}

std::filesystem::path SaveManager::defaultSavePath() {
    std::filesystem::path dataRoot;

#ifdef _WIN32
    dataRoot = getEnvironmentPath(L"LOCALAPPDATA");
    if (dataRoot.empty()) dataRoot = getEnvironmentPath(L"APPDATA");
#elif defined(__APPLE__)
    if (const char* userHome = std::getenv("HOME")) {
        dataRoot = std::filesystem::path(userHome)
            / "Library" / "Application Support";
    }
#else
    if (const char* xdgDataHome = std::getenv("XDG_DATA_HOME")) {
        dataRoot = xdgDataHome;
    } else if (const char* userHome = std::getenv("HOME")) {
        dataRoot = std::filesystem::path(userHome) / ".local" / "share";
    }
#endif

    if (dataRoot.empty()) {
        std::error_code error;
        dataRoot = std::filesystem::current_path(error);
        if (error) return std::filesystem::path(SAVE_FILE_NAME);
    }

    return dataRoot / GAME_DATA_DIRECTORY / SAVE_FILE_NAME;
}

std::filesystem::path SaveManager::existingSavePath() {
    const std::filesystem::path preferred = defaultSavePath();
    if (isRegularFile(preferred)) return preferred;

    // Older builds wrote next to whichever working directory launched the
    // game. Check that location and two parents so Debug/Release builds can
    // still discover an existing repository-root save.
    std::error_code error;
    std::filesystem::path directory = std::filesystem::current_path(error);
    if (!error) {
        for (int depth = 0; depth <= 2 && !directory.empty(); ++depth) {
            const std::filesystem::path legacy = directory / SAVE_FILE_NAME;
            if (isRegularFile(legacy)) return legacy;

            const std::filesystem::path parent = directory.parent_path();
            if (parent == directory) break;
            directory = parent;
        }
    }

    return preferred;
}

bool SaveManager::saveToFile(const std::filesystem::path& filePath,
                           const std::string& mapPath,
                           const std::string& tilesetPath,
                           const GameWorld& world,
                           const HUDManager& hud) 
{
    if (!world.hero()) {
        std::cerr << "[SaveManager] Failed to save: Hero instance is null!\n";
        return false;
    }

    std::cout << "[SaveManager] Starting game save process to '"
              << pathForLog(filePath) << "'...\n";

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

            if (auto* boss = dynamic_cast<ThorKing*>(enemy.get())) {
                eData.bossHp = boss->getBossHp();
                eData.fireCount = boss->getFireCount();
                eData.wallBounceCount = boss->getWallBounceCount();
                eData.shotSeq = boss->getShotSeq();
                eData.isSkyLaunching = boss->isSkyLaunching();
                eData.groundY = boss->getGroundY();
            }

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
        json eJson = {
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
        };

        if (enemy.type == "ThorKing" || enemy.type == "thorking" || enemy.type == "thor_king") {
            eJson["bossHp"] = enemy.bossHp;
            eJson["fireCount"] = enemy.fireCount;
            eJson["wallBounceCount"] = enemy.wallBounceCount;
            eJson["shotSeq"] = enemy.shotSeq;
            eJson["isSkyLaunching"] = enemy.isSkyLaunching;
            eJson["groundY"] = enemy.groundY;
        }

        j["aliveEnemies"].push_back(eJson);
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

    const std::filesystem::path destination(filePath);
    if (destination.empty()) {
        std::cerr << "[SaveManager] Error: Save path is empty.\n";
        return false;
    }

    std::error_code fileError;
    const std::filesystem::path parent = destination.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent, fileError);
        if (fileError) {
            std::cerr << "[SaveManager] Error creating save directory: "
                      << fileError.message() << "\n";
            return false;
        }
    }

    std::filesystem::path temporary = destination;
    temporary += ".tmp";
    std::filesystem::remove(temporary, fileError);
    fileError.clear();

    std::ofstream outFile(temporary, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open file for writing: "
                  << pathForLog(temporary) << "\n";
        return false;
    }
    outFile << j.dump(4);
    outFile.flush();
    if (!outFile.good()) {
        std::cerr << "[SaveManager] Error writing save file: "
                  << pathForLog(temporary) << "\n";
        outFile.close();
        std::filesystem::remove(temporary, fileError);
        return false;
    }
    outFile.close();
    if (outFile.fail()) {
        std::cerr << "[SaveManager] Error closing save file: "
                  << pathForLog(temporary) << "\n";
        std::filesystem::remove(temporary, fileError);
        return false;
    }

#ifdef _WIN32
    const bool replaced = MoveFileExW(
        temporary.c_str(), destination.c_str(),
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
    if (!replaced) {
        std::cerr << "[SaveManager] Error replacing save file (Windows error "
                  << GetLastError() << ").\n";
        std::filesystem::remove(temporary, fileError);
        return false;
    }
#else
    std::filesystem::rename(temporary, destination, fileError);
    if (fileError) {
        std::cerr << "[SaveManager] Error replacing save file: "
                  << fileError.message() << "\n";
        std::filesystem::remove(temporary, fileError);
        return false;
    }
#endif

    std::cout << "[SaveManager] Game saved successfully to '"
              << pathForLog(filePath) << "'! "
              << "(" << m_saveData.aliveEnemies.size() << " enemies, "
              << m_saveData.activeItems.size() << " active items, "
              << m_saveData.activeProjectiles.size() << " projectiles, "
              << m_saveData.hitBlocks.size() << " hit blocks, "
              << m_saveData.destroyedBlocks.size() << " destroyed blocks saved)\n";
    return true;
}

bool SaveManager::loadFromFile(const std::filesystem::path& filePath) {
    std::cout << "[SaveManager] Loading save data from file '"
              << pathForLog(filePath) << "'...\n";

    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[SaveManager] Error: Could not open save file: "
                  << pathForLog(filePath) << "\n";
        return false;
    }

    SaveData loadedData;
    try {
        json j;
        inFile >> j;
        if (!j.is_object()) {
            throw std::runtime_error("save root must be a JSON object");
        }

        loadedData.mapPath = j.value("mapPath", "");
        loadedData.tilesetPath = j.value("tilesetPath", "");
        if (loadedData.mapPath.empty() || loadedData.tilesetPath.empty()) {
            throw std::runtime_error("save is missing its level paths");
        }

        if (!j.contains("hero") || !j["hero"].is_object()) {
            throw std::runtime_error("save is missing valid hero data");
        }
        const auto& hero = j["hero"];
        loadedData.hero.posX = hero.value("posX", 0.0f);
        loadedData.hero.posY = hero.value("posY", 0.0f);
        loadedData.hero.hp = hero.value("hp", 1);
        loadedData.hero.coin = hero.value("coin", 0);
        loadedData.hero.heroType = hero.value("heroType", "Mario");
        loadedData.hero.formName = hero.value("formName", "SmallForm");
        loadedData.hero.invincibleTimer = hero.value("invincibleTimer", 0.0f);
        loadedData.hero.isStarman = hero.value("isStarman", false);
        if (loadedData.hero.heroType != "Mario"
            && loadedData.hero.heroType != "Luigi"
            && loadedData.hero.heroType != "Flash") {
            throw std::runtime_error("save contains an unknown hero type");
        }

        const auto requireArray = [&j](const char* key) -> const json* {
            if (!j.contains(key)) return nullptr;
            if (!j[key].is_array()) {
                throw std::runtime_error(std::string("save field '")
                    + key + "' must be an array");
            }
            return &j[key];
        };

        if (const json* enemies = requireArray("aliveEnemies")) {
            for (const auto& item : *enemies) {
                if (!item.is_object()) {
                    throw std::runtime_error("enemy save entry must be an object");
                }
                SaveData::EnemyData enemy;
                enemy.type = item.value("type", "Goomba");
                enemy.posX = item.value("posX", 0.0f);
                enemy.posY = item.value("posY", 0.0f);
                enemy.velX = item.value("velX", 0.0f);
                enemy.velY = item.value("velY", 0.0f);
                enemy.direction = item.value("direction", -1);
                enemy.health = item.value("health", 1);
                enemy.isGrounded = item.value("isGrounded", true);
                enemy.aiState = item.value("aiState", "Patrol");
                enemy.stateTimer = item.value("stateTimer", -1.0f);
                enemy.bossHp = item.value("bossHp", 3);
                enemy.fireCount = item.value("fireCount", 0);
                enemy.wallBounceCount = item.value("wallBounceCount", 0);
                enemy.shotSeq = item.value("shotSeq", 0);
                enemy.isSkyLaunching = item.value("isSkyLaunching", false);
                enemy.groundY = item.value("groundY", 0.0f);
                loadedData.aliveEnemies.push_back(enemy);
            }
        }

        const auto loadBlocks = [&requireArray](
                const char* key,
                std::vector<SaveData::BlockPosition>& destination) {
            if (const json* blocks = requireArray(key)) {
                for (const auto& item : *blocks) {
                    if (!item.is_object()) {
                        throw std::runtime_error("block save entry must be an object");
                    }
                    destination.push_back({
                        item.value("posX", 0.0f),
                        item.value("posY", 0.0f)
                    });
                }
            }
        };
        loadBlocks("hitBlocks", loadedData.hitBlocks);
        loadBlocks("destroyedBlocks", loadedData.destroyedBlocks);

        if (const json* items = requireArray("activeItems")) {
            for (const auto& item : *items) {
                if (!item.is_object()) {
                    throw std::runtime_error("item save entry must be an object");
                }
                loadedData.activeItems.push_back({
                    item.value("type", ""),
                    item.value("posX", 0.0f),
                    item.value("posY", 0.0f)
                });
            }
        }

        if (const json* projectiles = requireArray("activeProjectiles")) {
            for (const auto& item : *projectiles) {
                if (!item.is_object()) {
                    throw std::runtime_error("projectile save entry must be an object");
                }
                SaveData::ProjectileData projectile;
                projectile.type = item.value("type", "");
                projectile.posX = item.value("posX", 0.0f);
                projectile.posY = item.value("posY", 0.0f);
                projectile.velX = item.value("velX", 0.0f);
                projectile.velY = item.value("velY", 0.0f);
                const int faction = item.value("faction", 0);
                if (faction < 0 || faction > 1) {
                    throw std::runtime_error("projectile has an invalid faction");
                }
                projectile.faction = static_cast<ProjectileFaction>(faction);
                loadedData.activeProjectiles.push_back(projectile);
            }
        }

        if (j.contains("hud")) {
            if (!j["hud"].is_object()) {
                throw std::runtime_error("save contains invalid HUD data");
            }
            const auto& hud = j["hud"];
            loadedData.hud.score = hud.value("score", 0);
            loadedData.hud.coins = hud.value("coins", 0);
            loadedData.hud.lives = hud.value("lives", 3);
            loadedData.hud.remainingTime = hud.value(
                "remainingTime", 300.0f);
        }
    } catch (const std::exception& e) {
        m_saveData = SaveData{};
        std::cerr << "[SaveManager] Error parsing save file: "
                  << e.what() << "\n";
        return false;
    }

    m_saveData = std::move(loadedData);

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
            newHero->setPosition(m_saveData.hero.posX, m_saveData.hero.posY);
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
        } else if (eData.type == "ThorKing" || eData.type == "thorking" || eData.type == "thor_king") {
            eType = EnemyType::ThorKing;
            speed = 150.f;
        }

        auto enemy = EnemyFactory::createEnemy(eType, eData.posX, eData.posY, speed, spawnCallback);
        if (enemy) {
            MoveDirection dir = (eData.direction >= 0) ? MoveDirection::Right : MoveDirection::Left;
            enemy->setDirection(dir);
            enemy->setHealth(eData.health);
            enemy->setPosition({eData.posX, eData.posY});
            enemy->setGrounded(eData.isGrounded);

            if (auto* boss = dynamic_cast<ThorKing*>(enemy.get())) {
                boss->restoreBossState(
                    eData.bossHp, 
                    eData.fireCount, 
                    eData.wallBounceCount, 
                    eData.shotSeq, 
                    eData.isSkyLaunching, 
                    eData.groundY
                );
                
                boss->changeState(std::make_unique<TKPatrolState>(2.0f));

                if (std::abs(eData.velY) > 0.01f) {
                    boss->setVelocity({ boss->getVelocity().x, eData.velY });
                }
            } 
            else {
                auto restoredState = EnemyStateFactory::createStateFromString(eData.aiState, eData.stateTimer);
                if (restoredState) {
                    enemy->changeState(std::move(restoredState));
                }
                
                enemy->setVelocity({eData.velX, eData.velY});
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
            item->setActive(true);
            if (auto* coin = dynamic_cast<Coin*>(item.get())) {
                coin->spawnAsGroundCoin();
            }
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
