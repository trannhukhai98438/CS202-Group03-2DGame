#include "Core/PlayingState.h"

#include "Core/Game.h"
#include "Core/GameOverState.h"
#include "Core/PausedState.h"
#include "Core/TransitionState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroState/CheerState.h"

#include "Managers/SaveManager.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>

namespace {
constexpr float VICTORY_DELAY_SECONDS = 0.75f;
constexpr float CAMERA_WIDTH = 1280.0f;
constexpr float CAMERA_HEIGHT = 720.0f;
constexpr float UNDERGROUND_ROOM_LEFT = 1536.0f;
constexpr float UNDERGROUND_ROOM_TOP = 544.0f;
constexpr float UNDERGROUND_ROOM_WIDTH = 512.0f;
constexpr float UNDERGROUND_ROOM_HEIGHT = 416.0f;

bool isPipeDirectionHeld(PipeDirection direction) {
    switch (direction) {
    case PipeDirection::Down:
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    case PipeDirection::Up:
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    case PipeDirection::Left:
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    case PipeDirection::Right:
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    case PipeDirection::None:
        return false;
    }

    return false;
}

PipeDirection getHeldPipeDirection() {
    if (isPipeDirectionHeld(PipeDirection::Down)) {
        return PipeDirection::Down;
    }
    if (isPipeDirectionHeld(PipeDirection::Up)) {
        return PipeDirection::Up;
    }
    if (isPipeDirectionHeld(PipeDirection::Left)) {
        return PipeDirection::Left;
    }
    if (isPipeDirectionHeld(PipeDirection::Right)) {
        return PipeDirection::Right;
    }
    return PipeDirection::None;
}

float getCameraY(float activeRegionBottom) {
    return activeRegionBottom - CAMERA_HEIGHT * 0.5f;
}
}

PlayingState::PlayingState(std::shared_ptr<HUDManager> hudManager)
	: m_hudManager(std::move(hudManager)),
	  m_levelRuntime("assets/maps/levels/1-1.tmj",
	                     "assets/maps/resources/tileset.png",
	                     Game::getInstance().getSelectedHero()) {
    if (!m_hudManager) {
        m_hudManager = std::make_shared<HUDManager>();
    }
    m_camera.setSize(CAMERA_WIDTH, CAMERA_HEIGHT);
    m_camera.setCenter(
        CAMERA_WIDTH * 0.5f,
        getCameraY(m_levelRuntime.getActiveRegionBottom()));
    m_hudManager->init("assets/fonts/SuperMario256.ttf");
    m_attemptStartScore = m_hudManager->getScore();
    m_attemptStartCoins = m_hudManager->getCoins();
    m_attemptStartLives = m_hudManager->getLives();

    if (!m_levelRuntime.isReady()) {
        std::cerr << "[PlayingState] ERROR: Cannot initialize level runtime!"
                  << std::endl;
    }

    const Game& game = Game::getInstance();
    m_soundManager.setBGMVolume(game.getThemeMusicVolume());
    m_soundManager.setSFXVolume(game.getSfxVolume());
    m_soundManager.loadAllSFX();
    m_soundManager.playBGM("ground");
}

PlayingState::~PlayingState() = default;

bool PlayingState::updateTimer(float deltaTime) {
    m_hudManager->updateTimer(deltaTime);
    if (m_hudManager->getRemainingTime() > 0.0f) return false;

    Game::getInstance().changeState(std::make_unique<GameOverState>());
    return true;
}

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        // --- Pause Game (TAB Key) ---
        if (event.key.code == sf::Keyboard::Tab) {
            Game::getInstance().pushState(std::make_unique<PausedState>());
            return;
        }
        // --- Save Game (F5) ---
        if (event.key.code == sf::Keyboard::F5) {
            quickSave();
            return;
        }
        // --- Load Game (F9) ---
        if (event.key.code == sf::Keyboard::F9) {
            quickLoad();
            return;
        }
    }
}

void PlayingState::update(sf::Time dt) {
    const float deltaTime = dt.asSeconds();

    const PipeDirection heldPipeDirection = getHeldPipeDirection();
    if (m_latchedPipeDirection != PipeDirection::None
        && !isPipeDirectionHeld(m_latchedPipeDirection)) {
        m_latchedPipeDirection = PipeDirection::None;
    }
    const PipeDirection requestedPipeDirection =
        (m_latchedPipeDirection == PipeDirection::None
         && !m_victoryPending && !m_defeatPending)
            ? heldPipeDirection
            : PipeDirection::None;

    const LevelUpdateResult levelUpdate =
        m_levelRuntime.update(deltaTime, requestedPipeDirection);
    if (!m_defeatPending && levelUpdate.scoreDelta > 0) {
        m_hudManager->addScore(levelUpdate.scoreDelta);
    }
    if (levelUpdate.travelledThroughPipe) {
        m_latchedPipeDirection = requestedPipeDirection;
        m_soundManager.playSFX("pipe");
        const bool isUnderground =
            m_levelRuntime.getActiveRegionBottom() > CAMERA_HEIGHT;
        m_soundManager.playBGM(isUnderground ? "underground" : "ground");
    }

    Hero* hero = m_levelRuntime.getHero();
    if (!hero) {
        updateTimer(deltaTime);
        return;
    }

    const int currentCoins = hero->getCoin();
    if (!m_defeatPending && currentCoins > m_lastCoinCount) {
        const int difference = currentCoins - m_lastCoinCount;
        m_hudManager->addCoin(difference);
        m_hudManager->addScore(100 * difference);
        m_soundManager.playSFX("coin");
        m_lastCoinCount = currentCoins;
    }

    const float halfScreenWidth = CAMERA_WIDTH * 0.5f;
    const float levelEnd = m_levelRuntime.getWorldWidth();
    const float cameraX = std::clamp(hero->getPosition().x,
                                     halfScreenWidth,
                                     levelEnd - halfScreenWidth);
    // Keep the world aligned to physical pixels. Following the Hero's
    // fractional X position directly makes tightly packed atlas tiles sample
    // neighbouring texels, which appears as one-pixel vertical seams.
    // Align the active room's bottom with the bottom of the viewport. On the
    // surface this prevents the vertically stacked underground map from being
    // visible; pipe travel still moves the camera to that room after entry.
    const float cameraY = getCameraY(
        m_levelRuntime.getActiveRegionBottom());
    m_camera.setCenter(std::round(cameraX), std::round(cameraY));

    // Goal activation is resolved by InteractionSystem. PlayingState only
    // owns the presentation delay and the game-state transition.
    if (!m_victoryPending && m_levelRuntime.hasActivatedGoal()) {
        hero->setState(std::make_unique<CheerState>());
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

    // Victory is resolved before the timer so touching the goal freezes the
    // HUD immediately and cannot be overwritten by a timeout in this frame.
    if (updateTimer(deltaTime)) return;

    if (!m_defeatPending && hero->isDead()) {
        m_hudManager->restoreProgress(
            m_attemptStartScore,
            m_attemptStartCoins,
            m_attemptStartLives);
        m_hudManager->loseLife();
        m_defeatPending = true;
        m_defeatDelayRemaining = DEFEAT_DELAY_SECONDS;
    } else if (m_defeatPending) {
        m_defeatDelayRemaining -= deltaTime;
        if (m_defeatDelayRemaining <= 0.0f) {
			if (m_hudManager->getLives() > 0) {
				Game::getInstance().changeState(
					std::make_unique<TransitionState>(m_hudManager));
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
    if (m_levelRuntime.getActiveRegionBottom() > CAMERA_HEIGHT) {
        sf::RectangleShape undergroundBackdrop({
            UNDERGROUND_ROOM_WIDTH, UNDERGROUND_ROOM_HEIGHT
        });
        undergroundBackdrop.setPosition(
            UNDERGROUND_ROOM_LEFT, UNDERGROUND_ROOM_TOP);
        undergroundBackdrop.setFillColor(sf::Color::Black);
        window.draw(undergroundBackdrop);
    }
    m_levelRuntime.renderWorld(window);
    window.draw(*m_hudManager);
    window.setView(window.getDefaultView());
}

void PlayingState::quickSave() {
    SaveManager saveManager;
    GameWorld& world = m_levelRuntime.getWorld();

    saveManager.saveToFile(
        "savegame.json",
        m_levelRuntime.getMapPath(),
        m_levelRuntime.getTilesetPath(),
        m_levelRuntime.getWorld(),
        *m_hudManager
    );
}

void PlayingState::quickLoad() {
    SaveManager saveManager;
    if (!saveManager.loadFromFile("savegame.json", m_levelRuntime.getWorld())) return;

    m_levelRuntime.reload(
        saveManager.getSaveData().mapPath,
        saveManager.getSaveData().tilesetPath,
        Game::getInstance().getSelectedHero()
    );

    if (saveManager.applySaveToWorld(m_levelRuntime.getWorld(), *m_hudManager)) {
        if (Hero* hero = m_levelRuntime.getHero()) {
            m_lastCoinCount = hero->getCoin();

            // SaveManager replaces the Hero instance, so restore the exact
            // playable room before camera, theme and fall-death are evaluated.
            m_levelRuntime.syncActiveRegionToHero();
            m_levelRuntime.update(0.0f, PipeDirection::None);

            const float halfScreenWidth = CAMERA_WIDTH * 0.5f;
            const float levelEnd = m_levelRuntime.getWorldWidth();
            const float cameraX = std::clamp(hero->getPosition().x,
                                              halfScreenWidth,
                                              levelEnd - halfScreenWidth);
            const float cameraY = getCameraY(m_levelRuntime.getActiveRegionBottom());

            m_camera.setCenter(std::round(cameraX), std::round(cameraY));
        }
    }
}
