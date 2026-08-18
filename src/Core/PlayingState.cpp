#include "Core/PlayingState.h"

#include "Core/Game.h"
#include "Core/GameOverState.h"
#include "Core/PausedState.h"
#include "Core/TransitionState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroState/FlyState.h"

#include <algorithm>
#include <iostream>
#include <memory>

namespace {
constexpr float VICTORY_DELAY_SECONDS = 0.75f;
}

PlayingState::PlayingState()
	: m_levelRuntime("assets/maps/levels/1-1.tmj",
	                     "assets/maps/resources/tileset.png",
	                     Game::getInstance().getSelectedHero()) {
    m_camera.setSize(1280.0f, 720.0f);
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
	m_hudManager.setLives(Game::getInstance().getLives());

    if (!m_levelRuntime.isReady()) {
        std::cerr << "[PlayingState] ERROR: Cannot initialize level runtime!"
                  << std::endl;
    }
}

PlayingState::~PlayingState() = default;

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
}

void PlayingState::update(sf::Time dt) {
    const float deltaTime = dt.asSeconds();

    // The timer intentionally continues through victory and defeat delays.
    m_hudManager.updateTimer(deltaTime);
    if (m_hudManager.getRemainingTime() <= 0.0f) {
        // Time-out behavior remains deferred until the lives/session phase.
    }

    const int scoreDelta = m_levelRuntime.update(deltaTime);
    if (scoreDelta > 0) {
        m_hudManager.addScore(scoreDelta);
    }

    Hero* hero = m_levelRuntime.getHero();
    if (!hero) return;

    const int currentCoins = hero->getCoin();
    if (currentCoins > m_lastCoinCount) {
        const int difference = currentCoins - m_lastCoinCount;
		const int livesBeforeCoins = m_hudManager.getLives();
        m_hudManager.addCoin(difference);
		if (m_hudManager.getLives() > livesBeforeCoins) {
			Game::getInstance().addLife();
		}
        m_hudManager.addScore(100 * difference);
        m_lastCoinCount = currentCoins;
    }

    const float halfScreenWidth = 640.0f;
    const float levelEnd = m_levelRuntime.getWorldWidth();
    const float cameraX = std::clamp(hero->getPosition().x,
                                     halfScreenWidth,
                                     levelEnd - halfScreenWidth);
    m_camera.setCenter(cameraX, 360.0f);

    // Goal activation is resolved by InteractionSystem. PlayingState only
    // owns the presentation delay and the game-state transition.
    if (!m_victoryPending && m_levelRuntime.hasActivatedGoal()) {
        hero->setState(std::make_unique<FlyState>());
        m_victoryPending = true;
        m_victoryDelayRemaining = VICTORY_DELAY_SECONDS;
    } else if (m_victoryPending) {
        m_victoryDelayRemaining -= deltaTime;
        if (m_victoryDelayRemaining <= 0.0f) {
            const sf::FloatRect heroBounds = hero->getBounds();
            const sf::Vector2f cameraTopLeft = {
                m_camera.getCenter().x - m_camera.getSize().x / 2.f,
                m_camera.getCenter().y - m_camera.getSize().y / 2.f
            };
            sf::Vector2f heroScreenPosition = {
                heroBounds.left + heroBounds.width / 2.f - cameraTopLeft.x,
                heroBounds.top + heroBounds.height / 2.f - cameraTopLeft.y
            };
            heroScreenPosition.x = std::clamp(
                heroScreenPosition.x, 48.f, 1232.f);
            heroScreenPosition.y = std::clamp(
                heroScreenPosition.y, 48.f, 672.f);
            Game::getInstance().pushState(
                std::make_unique<VictoryState>(heroScreenPosition));
            return;
        }
    }

    // Once victory is pending it remains authoritative, even if the ongoing
    // simulation makes the Hero dead during the transition delay.
    if (m_victoryPending) return;

    if (!m_defeatPending && hero->isDead()) {
		Game::getInstance().loseLife();
		m_hudManager.setLives(Game::getInstance().getLives());
        m_defeatPending = true;
        m_defeatDelayRemaining = DEFEAT_DELAY_SECONDS;
    } else if (m_defeatPending) {
        m_defeatDelayRemaining -= deltaTime;
        if (m_defeatDelayRemaining <= 0.0f) {
			if (Game::getInstance().getLives() > 0) {
				Game::getInstance().changeState(
					std::make_unique<TransitionState>());
			} else {
				Game::getInstance().changeState(
					std::make_unique<GameOverState>());
			}
            return;
        }
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.setView(m_camera);
    m_levelRuntime.renderWorld(window);
    window.draw(m_hudManager);
    window.setView(window.getDefaultView());
}
