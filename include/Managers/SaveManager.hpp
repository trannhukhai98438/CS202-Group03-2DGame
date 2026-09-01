#pragma once

#include "Entities/Character/Enemy/Projectile.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

class GameWorld;
class Hero;
class Enemy;
class Block;
class Item;
class HUDManager;
class LevelManager;

struct SaveData {
    std::string mapPath;
    std::string tilesetPath;

    // Hero state
    struct HeroData {
        float posX{0.0f};
        float posY{0.0f};
        int hp{1};
        int coin{0};
        std::string heroType = "Mario";
        std::string formName{"SmallForm"};
        float invincibleTimer{0.0f};
        bool isStarman{false};
    } hero;

    // Enemy state
    struct EnemyData {
        std::string type;
        float posX{0.0f};
        float posY{0.0f};
        float velX{0.0f};
        float velY{0.0f};
        int direction{-1};
        int health{1};
        bool isGrounded{true};
        std::string aiState{"Patrol"}; // AI state name (e.g., "Patrol", "Squished", "Shell")
        float stateTimer{-1.0f};      // Timer for timed states (e.g., SquishedState duration)
    };
    std::vector<EnemyData> aliveEnemies;

    // Block tracking (Hit vs Destroyed)
    struct BlockPosition {
        float posX{0.0f};
        float posY{0.0f};
    };
    std::vector<BlockPosition> hitBlocks;
    std::vector<BlockPosition> destroyedBlocks;

    // Active Items tracking
    struct ItemData {
        std::string type;
        float posX{0.0f};
        float posY{0.0f};
    };
    std::vector<ItemData> activeItems;

    // Active Projectiles tracking
    struct ProjectileData {
        std::string type;
        float posX{0.0f};
        float posY{0.0f};
        float velX{0.0f};
        float velY{0.0f};
        ProjectileFaction faction{ProjectileFaction::Hero};
    };
    std::vector<ProjectileData> activeProjectiles;

    // HUD metrics
    struct HUDData {
        int score{0};
        int coins{0};
        int lives{3};
        float remainingTime{300.0f};
    } hud;
};

class SaveManager {
public:
    SaveManager() = default;

    bool saveToFile(const std::string& filePath,
                const std::string& mapPath,
                const std::string& tilesetPath,
                const GameWorld& world,
                const HUDManager& hud);

    bool loadFromFile(const std::string& filePath, GameWorld& world);

    bool applySaveToWorld(GameWorld& world, HUDManager& hud) const;

    const SaveData& getSaveData() const { return m_saveData; }

private:
    SaveData m_saveData;
};