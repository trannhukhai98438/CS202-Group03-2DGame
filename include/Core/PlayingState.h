#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Managers/HUDManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class PlayingState : public State {
private:
    std::unique_ptr<Character> m_hero;
    sf::RectangleShape m_dummyFloor;
    float m_velocityX;
    float m_velocityY;
    sf::RectangleShape m_dummyWall;
    PhysicsEngine m_physics;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    HUDManager m_hudManager;
public:
    PlayingState();
    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};