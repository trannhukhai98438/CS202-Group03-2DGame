#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class TransitionState : public State {
private:
	sf::Font m_font;
	sf::Text m_worldText;
	sf::Text m_livesText;
	sf::Time m_elapsedTime; //Track how long the transition has been displayed
public:
	TransitionState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};