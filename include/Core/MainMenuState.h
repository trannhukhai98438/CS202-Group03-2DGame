#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

// State Pattern: MainMenuState inherits from State and implements its own behavior 
// for processing events, updating, and rendering.

class MainMenuState : public State {
private:
	sf::Font m_font;
	sf::Text m_titleText;
	sf::Text m_promptText;
public:
	MainMenuState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};