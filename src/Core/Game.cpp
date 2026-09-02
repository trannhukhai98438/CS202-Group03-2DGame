#include "Core/Game.h"

#include <algorithm>
#include <utility>

Game::Game() : m_window(sf::VideoMode(1280, 720), "Super Mario - Custom Engine", sf::Style::Titlebar | sf::Style::Close) {
	m_soundManager.setBGMVolume(m_themeMusicVolume);
    m_soundManager.setSFXVolume(m_sfxVolume);
    m_soundManager.loadAllSFX();
}

SoundManager& Game::getSoundManager() {
    return m_soundManager;
}

const SoundManager& Game::getSoundManager() const {
    return m_soundManager;
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const sf::Time maxFrameTime = sf::seconds(0.25f);
	// main() queues the initial state before entering the game loop.
	applyPendingStateAction();

	while (m_window.isOpen()) {
		// Loading a large level must not create seconds of fixed-update debt.
		// Discard excess wall-clock time after a stall so the game remains
		// responsive instead of trying to replay every missed frame.
		sf::Time elapsedTime = std::min(clock.restart(), maxFrameTime);
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
    m_soundManager.setBGMVolume(m_themeMusicVolume);
}

void Game::setSfxVolume(float volume) {
    m_sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    m_soundManager.setSFXVolume(m_sfxVolume);
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
