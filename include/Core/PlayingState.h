#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Managers/HUDManager.hpp"
#include <SFML/Graphics.hpp>

class PlayingState : public State {
private:
	sf::RectangleShape m_dummyMario;
	sf::RectangleShape m_dummyFloor;
	float m_velocityX;
	float m_velocityY;
	sf::RectangleShape m_dummyWall;
	PhysicsEngine m_physics;

	HUDManager m_hudManager;
public:
	PlayingState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};