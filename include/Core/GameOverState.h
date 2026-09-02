#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class GameOverState : public State {
private:
	sf::Font m_font;
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;
	sf::Texture m_heroTexture;
	sf::Sprite m_heroSprite;
	sf::Text m_gameOverText;
	sf::Text m_promptText;
	float m_fallVelocity{80.f};
	float m_landingY{580.f};
	bool m_hasLanded{false};
public:
	GameOverState();
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};
