#include "Core/Game.h"

#include <algorithm>
#include <utility>

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

		bool repeatedActivationKey = false;
		if (event.type == sf::Event::LostFocus) {
			m_tabKeyDown = false;
			m_enterKeyDown = false;
		} else if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Tab) {
				m_tabKeyDown = false;
			} else if (event.key.code == sf::Keyboard::Enter) {
				m_enterKeyDown = false;
			}
		} else if (event.type == sf::Event::KeyPressed) {
			bool* keyDown = nullptr;
			if (event.key.code == sf::Keyboard::Tab) {
				keyDown = &m_tabKeyDown;
			} else if (event.key.code == sf::Keyboard::Enter) {
				keyDown = &m_enterKeyDown;
			}
			if (keyDown) {
				repeatedActivationKey = *keyDown;
				*keyDown = true;
			}
		}
		// Once a state requests a transition, do not send later events from the
		// same poll batch to the outgoing state. Window-close events are still
		// handled above.
		if (!repeatedActivationKey
			&& m_pendingStateAction == PendingStateAction::None
			&& !m_states.empty()) {
			m_states.back()->processEvents(event);
		}
	}
}

void Game::update(sf::Time dt) {
	// State requests made by the previous event/update phase are committed
	// here, outside every State callback. The newly activated state then gets
	// one update before it can ever be rendered.
	applyPendingStateAction();
	if (!m_states.empty()) {
		m_states.back()->update(dt);
	}
}

void Game::render() {
	m_window.clear(sf::Color(92, 148, 252));
	if (!m_states.empty()) {
		std::size_t firstStateToRender = m_states.size() - 1;
		while (firstStateToRender > 0
			&& m_states[firstStateToRender]->rendersBelow()) {
			--firstStateToRender;
		}
		for (std::size_t i = firstStateToRender; i < m_states.size(); ++i) {
			m_states[i]->render(m_window);
		}
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

void Game::clearStatesAndChange(std::unique_ptr<State> state) {
	m_pendingState = std::move(state);
	m_pendingStateAction = PendingStateAction::ClearAndPush;
}

void Game::setThemeMusicVolume(float volume) {
	m_themeMusicVolume = std::clamp(volume, 0.0f, 100.0f);
}

void Game::setSfxVolume(float volume) {
	m_sfxVolume = std::clamp(volume, 0.0f, 100.0f);
}

void Game::setSelectedLevel(std::string levelPath, std::string worldName) {
	if (levelPath.empty() || worldName.empty()) return;
	m_selectedLevelPath = std::move(levelPath);
	m_selectedWorldName = std::move(worldName);
}

void Game::applyPendingStateAction() {
	const PendingStateAction action = m_pendingStateAction;
	m_pendingStateAction = PendingStateAction::None;

	switch (action) {
	case PendingStateAction::Push:
		if (m_pendingState) {
			m_states.push_back(std::move(m_pendingState));
		}
		break;
	case PendingStateAction::Pop:
		if (!m_states.empty()) {
			m_states.pop_back();
		}
		break;
	case PendingStateAction::Replace:
		if (!m_states.empty()) {
			m_states.pop_back();
		}
		if (m_pendingState) {
			m_states.push_back(std::move(m_pendingState));
		}
		break;
	case PendingStateAction::ClearAndPush:
		m_states.clear();
		if (m_pendingState) {
			m_states.push_back(std::move(m_pendingState));
		}
		break;
	case PendingStateAction::None:
		break;
	}
}
