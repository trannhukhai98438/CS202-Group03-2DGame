#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class PlayingState : public State {
private:
	sf::Font m_font;
	sf::Text m_placeholderText;
public:
	PlayingState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;

};