#include "Core/GameOverState.h"
#include "Core/Game.h"
#include "Core/MainMenuState.h"
#include "Entities/Character/Hero/HeroFactory.h"

#include <iostream>

GameOverState::GameOverState() {
	Game::getInstance().getSoundManager().playBGM("game_over", false);

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

	const HeroType selectedHero = Game::getInstance().getSelectedHero();
	const char* heroTexturePath = selectedHero == HeroType::Luigi
		? "assets/textures/Luigi.png"
		: selectedHero == HeroType::Flash
			? "assets/textures/Flash.png"
			: "assets/textures/Mario.png";
	if (!m_heroTexture.loadFromFile(heroTexturePath)) {
		std::cerr << "Error loading game-over hero texture\n";
	}
	m_heroTexture.setSmooth(false);
	m_heroSprite.setTexture(m_heroTexture);
	// Dedicated small-character death/falling frame in sprite sheets.
	if (selectedHero == HeroType::Flash) {
		m_heroSprite.setTextureRect({618, 230, 152, 193});
		m_heroSprite.setOrigin(76.f, 193.f);
		m_heroSprite.setScale(0.90f, 0.90f);
	} else if (selectedHero == HeroType::Mario) {
		m_heroSprite.setTextureRect({851, 164, 156, 66});
		m_heroSprite.setOrigin(78.f, 66.f);
		m_heroSprite.setScale(0.85f, 0.85f);
	} else {
		m_heroSprite.setTextureRect({761, 415, 227, 107});
		m_heroSprite.setOrigin(113.5f, 107.f);
		m_heroSprite.setScale(0.80f, 0.80f);
	}
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
