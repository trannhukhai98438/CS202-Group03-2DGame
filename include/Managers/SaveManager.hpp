#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

// Forward declarations of your game classes
class Hero;
class Enemy;
class Block;
class HUDManager;
class LevelManager;
class LevelBuilder;
class GameWorld;

struct SaveData {
    // --- MAP & LEVEL DATA ---
    std::string mapPath;
    std::string tilesetPath;

    // --- HERO DATA ---
    struct HeroData {
        float posX{ 0.0f };
        float posY{ 0.0f };
        int hp{ 1 };
        int coin{ 0 };
        std::string formName;
        float invincibleTimer{ 0.0f };
        bool isStarman{ false };
    } hero;

    // --- ENEMY DATA ---
    struct EnemyData {
        std::string type;
        float posX{ 0.0f };
        float posY{ 0.0f };
        int direction{ -1 };
        int health{ 1 };
    };
    std::vector<EnemyData> aliveEnemies;

    // --- BLOCK DATA ---
    struct HitBlockData {
        float posX{ 0.0f };
        float posY{ 0.0f };
    };
    std::vector<HitBlockData> hitBlocks;

    // --- HUD & GAME PROGRESS DATA ---
    struct HUDData {
        int score{ 0 };
        int coins{ 0 };
        int lives{ 3 };
        float remainingTime{ 300.0f };
        std::string worldName{ "1-1" };
    } hud;
};

class SaveManager {
private:
    SaveData m_saveData;

public:
    SaveManager() = default;
    ~SaveManager() = default;

    // Captures runtime state from game objects and writes to a JSON file
    bool saveToFile(const std::string& filePath,
                    const std::string& mapPath,
                    const std::string& tilesetPath,
                    const Hero* hero,
                    const std::vector<std::unique_ptr<Enemy>>& enemies,
                    const std::vector<std::unique_ptr<Block>>& blocks,
                    const HUDManager& hud);

    // Reads JSON file and populates the internal SaveData struct
    bool loadFromFile(const std::string& filePath);

    // Applies loaded state back to runtime game systems
    bool applySaveToWorld(GameWorld& world, HUDManager& hud) const; 

    // Getters
    const SaveData& getSaveData() const { return m_saveData; }
};