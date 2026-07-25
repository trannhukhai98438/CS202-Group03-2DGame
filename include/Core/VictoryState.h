#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class VictoryState : public State {
private:
	sf::Font m_font;
	sf::Text m_victoryText;
	sf::Text m_promptText;
public:
	VictoryState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};