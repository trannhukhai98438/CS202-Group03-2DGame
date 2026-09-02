#include "Core/TransitionState.h"
#include "Core/Game.h"
#include "Core/PlayingState.h"
#include "Managers/HUDManager.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

TransitionState::TransitionState()
	: TransitionState(
		std::make_shared<HUDManager>(),
		Game::getInstance().getSelectedLevelPath(),
		Game::getInstance().getSelectedWorldName()) {
}

TransitionState::TransitionState(std::shared_ptr<HUDManager> hudManager,
                                 const std::string& levelPath,
                                 const std::string& worldName,
                                 bool resumeSavedProgress)
	: m_hudManager(std::move(hudManager)),
	  m_levelPath(levelPath),
	  m_resumeSavedProgress(resumeSavedProgress),
	  m_elapsedTime(sf::Time::Zero) {
	if (!m_hudManager) {
		m_hudManager = std::make_shared<HUDManager>();
	}
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "ERROR: Failed to load font!\n";
	}
	m_worldText.setFont(m_font);
	m_worldText.setString(worldName);
	m_worldText.setCharacterSize(40);
	m_worldText.setFillColor(sf::Color::White);
	m_livesText.setFont(m_font);
	m_livesText.setString("LIVES: "
		+ std::to_string(m_hudManager->getLives()));
	m_livesText.setCharacterSize(40);
	m_livesText.setFillColor(sf::Color::White);
}

void TransitionState::processEvents(sf::Event& event) {
	// No event processing needed for this state.
}

void TransitionState::update(sf::Time dt) {
	m_elapsedTime += dt;
	if (m_elapsedTime.asSeconds() > 2.0f) { // After 2.0 seconds, transition to the PlayingState
		Game::getInstance().changeState(
			std::make_unique<PlayingState>(
				m_hudManager, m_levelPath, m_resumeSavedProgress));
	}
}

void TransitionState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Black);
	m_worldText.setPosition((window.getSize().x - m_worldText.getLocalBounds().width) / 2.f, 250.f);
	m_livesText.setPosition((window.getSize().x - m_livesText.getLocalBounds().width) / 2.f, 350.f);
	window.draw(m_worldText);
	window.draw(m_livesText);
}
