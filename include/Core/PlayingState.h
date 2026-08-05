#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include "Managers/HUDManager.hpp"
#include "Managers/LevelManager.hpp"
#include <SFML/Graphics.hpp>

class PlayingState : public State {
private:
    sf::RectangleShape m_mario;
    sf::Vector2f m_velocity;

	HUDManager m_hudManager;
	LevelManager m_levelManager;
public:
	PlayingState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};