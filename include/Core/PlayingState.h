#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Block/BlockFactory.h"
#include "Entities/Item/ItemFactory.h"
#include "Managers/HUDManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Hero;
class Item;
class Block;

class PlayingState : public State {
private:
    std::unique_ptr<Hero> hero;
    std::vector<std::unique_ptr<Item>> items;
    std::vector<std::unique_ptr<Block>> blocks;
    sf::RectangleShape m_dummyFloor;
    sf::RectangleShape m_dummyWall;
    PhysicsEngine m_physics;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    HUDManager m_hudManager;
public:
    PlayingState();
    ~PlayingState() override;
    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};