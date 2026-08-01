#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Entities/EnemyFactory.h"
#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class PlayingState : public State {
private:
	sf::RectangleShape m_dummyMario;
	sf::RectangleShape m_dummyFloor;
	float m_velocityX;
	float m_velocityY;
	sf::RectangleShape m_dummyWall;
	PhysicsEngine m_physics;

	HUDManager m_hudManager;
	LevelManager m_levelManager;
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::vector<std::unique_ptr<Projectile>> m_projectiles;
public:
	PlayingState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};