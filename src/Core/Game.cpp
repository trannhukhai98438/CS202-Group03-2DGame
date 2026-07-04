#include "Core/Game.h"

Game::Game() : m_window(sf::VideoMode(800, 600), "Super Mario - Custom Engine")
{
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (m_window.isOpen()) {
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;

		while (timeSinceLastUpdate > TimePerFrame) {
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		render();
	}
}

void Game::processEvents() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_window.close();
		}
	}
}

void Game::update(sf::Time dt) {
	// TODO: State Pattern updates will go here.
	// This is where gravity applies.
}

void Game::render() {
	m_window.clear(sf::Color(92, 148, 252)); // Clear with a sky blue color
	//TODO: Draw the entities of the current State here.
	m_window.display();
}