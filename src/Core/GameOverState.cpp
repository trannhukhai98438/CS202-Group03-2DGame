#include "Core/GameOverState.h"
#include "Core/Game.h"
#include "Core/MainMenuState.h"
#include "Entities/Character/Hero/HeroFactory.h"

#include <iostream>

GameOverState::GameOverState() {
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "Error loading game-over font\n";
	}
	if (!m_backgroundTexture.loadFromFile(
			"assets/textures/gameOverBackground.png")) {
		std::cerr << "Error loading game-over background\n";
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);
	const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
	if (backgroundSize.x > 0 && backgroundSize.y > 0) {
		m_backgroundSprite.setScale(1280.f / backgroundSize.x,
		                            720.f / backgroundSize.y);
	}

	const bool isLuigi = Game::getInstance().getSelectedHero()
		== HeroType::Luigi;
	const char* heroTexturePath = isLuigi
		? "assets/textures/Luigi.png"
		: "assets/textures/Mario.png";
	if (!m_heroTexture.loadFromFile(heroTexturePath)) {
		std::cerr << "Error loading game-over hero texture\n";
	}
	m_heroTexture.setSmooth(false);
	m_heroSprite.setTexture(m_heroTexture);
	// Dedicated small-character death/falling frame in both sprite sheets.
	m_heroSprite.setTextureRect({116, 8, 16, 16});
	m_heroSprite.setOrigin(8.f, 16.f);
	m_heroSprite.setScale(6.f, 6.f);
	m_heroSprite.setPosition(640.f, -20.f);

	m_gameOverText.setFont(m_font);
	m_gameOverText.setString("GAME OVER");
	m_gameOverText.setCharacterSize(68);
	m_gameOverText.setFillColor(sf::Color(220, 35, 35));
	m_gameOverText.setOutlineColor(sf::Color::White);
	m_gameOverText.setOutlineThickness(3.f);
	const sf::FloatRect titleBounds = m_gameOverText.getLocalBounds();
	m_gameOverText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
	                         titleBounds.top + titleBounds.height / 2.f);
	m_gameOverText.setPosition(640.f, 105.f);

	m_promptText.setFont(m_font);
	m_promptText.setString("press ENTER to return to menu");
	m_promptText.setCharacterSize(19);
	m_promptText.setFillColor(sf::Color::White);
	const sf::FloatRect promptBounds = m_promptText.getLocalBounds();
	m_promptText.setOrigin(promptBounds.left + promptBounds.width / 2.f,
	                       promptBounds.top + promptBounds.height / 2.f);
	m_promptText.setPosition(640.f, 680.f);
}

void GameOverState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		Game::getInstance().changeState(std::make_unique<MainMenuState>());
	}
}

void GameOverState::update(sf::Time dt) {
	if (m_hasLanded) return;

	m_fallVelocity += 520.f * dt.asSeconds();
	const float nextY = m_heroSprite.getPosition().y
		+ m_fallVelocity * dt.asSeconds();
	if (nextY >= m_landingY) {
		m_heroSprite.setPosition(640.f, m_landingY);
		m_hasLanded = true;
	} else {
		m_heroSprite.setPosition(640.f, nextY);
	}
}

void GameOverState::render(sf::RenderWindow& window) {
	window.draw(m_backgroundSprite);
	window.draw(m_heroSprite);
	window.draw(m_gameOverText);
	window.draw(m_promptText);
}
