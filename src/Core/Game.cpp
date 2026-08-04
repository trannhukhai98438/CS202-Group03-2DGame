#include "Core/Game.h"

Game::Game() : m_window(sf::VideoMode(1280, 720), "Super Mario - Custom Engine", sf::Style::Titlebar | sf::Style::Close)
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
		if (!m_states.empty()) {
			m_states.top()->processEvents(event);
		}
	}
}

void Game::update(sf::Time dt) {
	if (!m_states.empty()) {
		m_states.top()->update(dt);
	}
}

void Game::render() {
	m_window.clear(sf::Color(92, 148, 252)); // Clear with a sky blue color
	if (!m_states.empty()) {
		m_states.top()->render(m_window);
	}
	m_window.display();
}

void Game::pushState(std::unique_ptr<State> state) {
	m_states.push(std::move(state));
}

void Game::popState() {
	if (!m_states.empty()) {
		m_states.pop();
	}
}

void Game::changeState(std::unique_ptr<State> state) {
	if (!m_states.empty()) {
		m_states.pop();
	}
	m_states.push(std::move(state));
}