#pragma once

#include "Managers/LevelManager.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <set>
#include <utility>

class Block;
class Enemy;
class Hero;
class Item;
class LevelGoal;
class Projectile;

class GameWorld {
public:
    GameWorld();
    ~GameWorld();

    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    LevelManager& levelManager();
    const LevelManager& levelManager() const;

    Hero* hero();
    const Hero* hero() const;
    void setHero(std::unique_ptr<Hero> hero);

    std::vector<std::unique_ptr<Block>>& blocks();
    const std::vector<std::unique_ptr<Block>>& blocks() const;

    std::vector<std::unique_ptr<Item>>& items();
    const std::vector<std::unique_ptr<Item>>& items() const;

    std::vector<std::unique_ptr<Enemy>>& enemies();
    const std::vector<std::unique_ptr<Enemy>>& enemies() const;

    std::vector<std::unique_ptr<Projectile>>& projectiles();
    const std::vector<std::unique_ptr<Projectile>>& projectiles() const;

    std::vector<std::unique_ptr<LevelGoal>>& goals();
    const std::vector<std::unique_ptr<LevelGoal>>& goals() const;

    std::vector<sf::RectangleShape>& mapColliders();
    const std::vector<sf::RectangleShape>& mapColliders() const;

    void addBlock(std::unique_ptr<Block> block);
    void addItem(std::unique_ptr<Item> item);
    void addEnemy(std::unique_ptr<Enemy> enemy);
    void addProjectile(std::unique_ptr<Projectile> projectile);
    void addGoal(std::unique_ptr<LevelGoal> goal);

    void addDestroyedBlock(int tx, int ty) { m_destroyedBlocks.insert({tx, ty}); }
    const std::set<std::pair<int, int>>& getDestroyedBlocks() const { return m_destroyedBlocks; }
    void setDestroyedBlocks(const std::set<std::pair<int, int>>& blocks) { m_destroyedBlocks = blocks; }

    void removeInactiveEntities();

    void clear();

private:
    LevelManager m_levelManager;
    std::unique_ptr<Hero> m_hero;
    std::vector<std::unique_ptr<Block>> m_blocks;
    std::vector<std::unique_ptr<Item>> m_items;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    std::vector<std::unique_ptr<LevelGoal>> m_goals;
    std::vector<sf::RectangleShape> m_mapColliders;
    std::set<std::pair<int, int>> m_destroyedBlocks;
};
