#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Hero;
class Item;
class Block;

class PlayingState : public State {
private:
    std::unique_ptr<Character> m_hero;    
    std::vector<std::unique_ptr<Item>> m_items;
	std::vector<std::unique_ptr<Block>> m_blocks;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    HUDManager m_hudManager;
    int m_lastCoinCount;
    LevelManager m_levelManager;
    sf::RectangleShape m_dummyWall;
    PhysicsEngine m_physics;
    sf::RectangleShape m_dummyFloor;
public:
	PlayingState();
	~PlayingState() override;
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};