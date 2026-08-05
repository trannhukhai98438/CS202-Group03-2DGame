#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Managers/HUDManager.hpp"
#include <SFML/Graphics.hpp>

class PlayingState : public State {
private:
	sf::View m_camera;
	sf::RectangleShape m_dummyMario; //TODO: Remove this when we have a proper Mario sprite and level assets
	sf::RectangleShape m_dummyFloor; //TODO: Remove this when we have a proper Mario sprite and level assets
	float m_velocityX; //TODO: Remove this when we have a proper Mario sprite and level assets
	float m_velocityY; //TODO: Remove this when we have a proper Mario sprite and level assets
	sf::RectangleShape m_dummyWall;//TODO: Remove this when we have a proper Mario sprite and level assets
	PhysicsEngine m_physics;

	HUDManager m_hudManager;
public:
	PlayingState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};