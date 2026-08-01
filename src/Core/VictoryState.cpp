#include "Core/VictoryState.h"
#include "Core/MainMenuState.h"
#include "Core/Game.h"

VictoryState::VictoryState() {
	m_font.loadFromFile("assets/fonts/SuperMario256.ttf");
	m_victoryText.setFont(m_font);
	m_victoryText.setString("VICTORY!");
	m_victoryText.setCharacterSize(60);
	m_victoryText.setFillColor(sf::Color::Yellow);
	m_promptText.setFont(m_font);
	m_promptText.setString("Press ENTER to continue");
	m_promptText.setCharacterSize(24);
	m_promptText.setFillColor(sf::Color::White);
}

void VictoryState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		// Later this can be changed to transition to the next level
		Game::getInstance().changeState(std::make_unique <MainMenuState>());
	}
}

void VictoryState::update(sf::Time dt) {}

void VictoryState::render(sf::RenderWindow& window) {
	window.clear(sf::Color(92, 148, 252));
	m_victoryText.setPosition((window.getSize().x - m_victoryText.getLocalBounds().width) / 2.f, 250.f);
	m_promptText.setPosition((window.getSize().x - m_promptText.getLocalBounds().width) / 2.f, 400.f);

	window.draw(m_victoryText);
	window.draw(m_promptText);
}