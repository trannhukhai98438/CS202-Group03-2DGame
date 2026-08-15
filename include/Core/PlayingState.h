#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Block/BlockFactory.h"
#include "Entities/Item/ItemFactory.h"
#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Hero;
class Item;
class Block;
class LevelGoal;

class PlayingState : public State {
private:
    sf::View m_camera; 
    std::unique_ptr<Hero> m_hero;    
    std::vector<std::unique_ptr<Item>> m_items;
    std::vector<std::unique_ptr<Block>> m_blocks;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    std::vector<std::unique_ptr<LevelGoal>> m_levelGoals;
    HUDManager m_hudManager;
    LevelManager m_levelManager;
    std::vector<sf::RectangleShape> m_mapColliders;
    PhysicsEngine m_physics;
    int m_lastCoinCount;
    bool m_victoryPending{false};
    float m_victoryDelayRemaining{0.0f};

    void updateProjectiles(float deltaTime);
    void simulateProjectile(Projectile& projectile, float deltaTime);
    void resolveProjectileTargets(Projectile& projectile);
public:
	PlayingState();
	~PlayingState() override;
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};
