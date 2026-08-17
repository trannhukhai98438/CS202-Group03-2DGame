#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>
#include <array>

// State Pattern: MainMenuState inherits from State and implements its own behavior 
// for processing events, updating, and rendering.

class MainMenuState : public State {
private:
	struct MenuButton {
		sf::RectangleShape shape;
		sf::Text label;
	};

	void updateButtonAppearance();
	int buttonAt(sf::Vector2f point) const;

	sf::Font m_font;
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;
	sf::RectangleShape m_overlay;
	sf::Text m_titleText;
	sf::Text m_statusText;
	std::array<MenuButton, 3> m_buttons;
	int m_hoveredButton = -1;
public:
	MainMenuState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};
