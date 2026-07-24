#include "Core/GameOverState.h"
#include "Core/Game.h"
#include "Core/MainMenuState.h"

GameOverState::GameOverState() {
	m_font.loadFromFile("assets/fonts/SuperMario256.ttf");
	m_gameOverText.setFont(m_font);
	m_gameOverText.setString("GAME OVER");
	m_gameOverText.setCharacterSize(60);
	m_gameOverText.setFillColor(sf::Color::Red);
	m_promptText.setFont(m_font);
	m_promptText.setString("Press ENTER to return to Menu");
	m_promptText.setCharacterSize(24);
	m_promptText.setFillColor(sf::Color::White);
}

void GameOverState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		Game::getInstance().changeState(std::make_unique<MainMenuState>());
	}
}

void GameOverState::update(sf::Time dt) {}

void GameOverState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Black);
	m_gameOverText.setPosition((window.getSize().x - m_gameOverText.getLocalBounds().width) / 2.f, 250.f);
	m_promptText.setPosition((window.getSize().x - m_promptText.getLocalBounds().width) / 2.f, 400.f);
	window.draw(m_gameOverText);
	window.draw(m_promptText);
}