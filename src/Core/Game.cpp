#include "Core/Game.h"

Game::Game() : m_window(sf::VideoMode(1280, 720), "Super Mario - Custom Engine", sf::Style::Titlebar | sf::Style::Close)
{
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	// main() queues the initial state before entering the game loop.
	applyPendingStateAction();

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
		// Once a state requests a transition, do not send later events from the
		// same poll batch to the outgoing state. Window-close events are still
		// handled above.
		if (m_pendingStateAction == PendingStateAction::None
			&& !m_states.empty()) {
			m_states.top()->processEvents(event);
		}
	}
}

void Game::update(sf::Time dt) {
	// State requests made by the previous event/update phase are committed
	// here, outside every State callback. The newly activated state then gets
	// one update before it can ever be rendered.
	applyPendingStateAction();
	if (!m_states.empty()) {
		m_states.top()->update(dt);
	}
}

void Game::render() {
	m_window.clear(sf::Color(92, 148, 252));
	if (!m_states.empty()) {
		m_states.top()->render(m_window);
	}
	m_window.display();
}

void Game::pushState(std::unique_ptr<State> state) {
	m_pendingState = std::move(state);
	m_pendingStateAction = PendingStateAction::Push;
}

void Game::popState() {
	m_pendingState.reset();
	m_pendingStateAction = PendingStateAction::Pop;
}

void Game::changeState(std::unique_ptr<State> state) {
	m_pendingState = std::move(state);
	m_pendingStateAction = PendingStateAction::Replace;
}

int Game::loseLife() {
	if (m_lives > 0) {
		--m_lives;
	}
	return m_lives;
}

void Game::applyPendingStateAction() {
	const PendingStateAction action = m_pendingStateAction;
	m_pendingStateAction = PendingStateAction::None;

	switch (action) {
	case PendingStateAction::Push:
		if (m_pendingState) {
			m_states.push(std::move(m_pendingState));
		}
		break;
	case PendingStateAction::Pop:
		if (!m_states.empty()) {
			m_states.pop();
		}
		break;
	case PendingStateAction::Replace:
		if (!m_states.empty()) {
			m_states.pop();
		}
		if (m_pendingState) {
			m_states.push(std::move(m_pendingState));
		}
		break;
	case PendingStateAction::None:
		break;
	}
}
