#include "Core/MainMenuState.h"
#include "Core/Game.h"
#include "Core/PlayingState.h"
#include "Core/TransitionState.h"
#include <iostream>

MainMenuState::MainMenuState() {
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "Error loading font\n";
	}

	m_titleText.setFont(m_font);
	m_titleText.setString("SUPER MARIO");
	m_titleText.setCharacterSize(60);
	m_titleText.setFillColor(sf::Color::White);
	m_titleText.setPosition(200.f, 150.f);

	m_promptText.setFont(m_font);
	m_promptText.setString("Press ENTER to Start");
	m_promptText.setCharacterSize(24);
	m_promptText.setFillColor(sf::Color::Yellow);
	m_promptText.setPosition(270.f, 300.f);
}

void MainMenuState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		std::cout << "ENTER pressed! Ready to load ...\n";
		Game::getInstance().changeState(std::make_unique<TransitionState>());
	}
}

void MainMenuState::update(sf::Time dt) {
	// For now: empty because menu has no moving parts.
}

void MainMenuState::render(sf::RenderWindow& window) {
	m_titleText.setPosition((window.getSize().x - m_titleText.getLocalBounds().width) / 2, 150.f);
	m_promptText.setPosition((window.getSize().x - m_promptText.getLocalBounds().width) / 2, 300.f);
	window.draw(m_titleText);
	window.draw(m_promptText);
}