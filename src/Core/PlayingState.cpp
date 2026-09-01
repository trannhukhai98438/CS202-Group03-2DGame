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

const char* getThemeBgm(MapTheme theme) {
    switch (theme) {
    case MapTheme::Underground:
        return "underground";
    case MapTheme::Castle:
        return "castle";
    case MapTheme::Unspecified:
    case MapTheme::Overworld:
        return "ground";
    }

    return "ground";
}
}

PlayingState::PlayingState(std::shared_ptr<HUDManager> hudManager)
	: m_hudManager(std::move(hudManager)),
	  m_levelRuntime("assets/maps/levels/1-3.tmj",
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

    Game& game = Game::getInstance();
    m_levelRuntime.setSoundManager(&game.getSoundManager());
    const MapTheme activeTheme = m_levelRuntime.getWorld()
                                     .blockThemePalette()
                                     .getActiveTheme();
    game.getSoundManager().playBGM(getThemeBgm(activeTheme), true);
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
            Game::getInstance().getSoundManager().playSFX("pause");
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
    Hero* hero = m_levelRuntime.getHero();
    if (!hero) {
        updateTimer(deltaTime);
        return;
    }

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
        Game::getInstance().getSoundManager().playSFX("pipe");

        const bool starmanActive = hero->getIsStarman() && hero->getInvincibleTimer() > 0.0f;
        if (!starmanActive) {
            const MapTheme activeTheme =
                m_levelRuntime.getWorld()
                    .blockThemePalette()
                    .getActiveTheme();

            Game::getInstance().getSoundManager().playBGM(
                getThemeBgm(activeTheme), true);
        }
    }

    const bool starmanActive = hero->getIsStarman() && hero->getInvincibleTimer() > 0.0f;
    if (starmanActive && !m_invincibilityBgmActive) {
        Game::getInstance().getSoundManager().stopBGM();
        Game::getInstance().getSoundManager().playBGM("invincibility", true);
        m_invincibilityBgmActive = true;
    } else if (!starmanActive && m_invincibilityBgmActive) {
        Game::getInstance().getSoundManager().stopBGM();

        const MapTheme activeTheme =
            m_levelRuntime.getWorld()
                .blockThemePalette()
                .getActiveTheme();

        Game::getInstance().getSoundManager().playBGM(
            getThemeBgm(activeTheme), true);

        m_invincibilityBgmActive = false;
    }

    const int currentCoins = hero->getCoin();
    if (!m_defeatPending && currentCoins > m_lastCoinCount) {
        const int difference = currentCoins - m_lastCoinCount;
        m_hudManager->addCoin(difference);
        m_hudManager->addScore(100 * difference);
        Game::getInstance().getSoundManager().playSFX("coin");
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
            m_invincibilityBgmActive = false;
            SoundManager& soundManager =
                Game::getInstance().getSoundManager();
            soundManager.stopBGM();
            const std::string completionBgm =
                m_levelRuntime.getActivatedGoalBgm();
            soundManager.playBGM(
                completionBgm.empty() ? "course_clear" : completionBgm,
                false);
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
        m_invincibilityBgmActive = false;
        Game::getInstance().getSoundManager().stopBGM();
        Game::getInstance().getSoundManager().playBGM("player_down", false);
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
    Game& game = Game::getInstance();

    if (!saveManager.loadFromFile("savegame.json", m_levelRuntime.getWorld())) return;

    m_levelRuntime.reload(
        saveManager.getSaveData().mapPath,
        saveManager.getSaveData().tilesetPath,
        game.getSelectedHero()
    );

    if (!m_levelRuntime.isReady()) {
        std::cerr << "[PlayingState] ERROR: Cannot reload saved level.\n";
        return;
    }

    m_levelRuntime.setSoundManager(&game.getSoundManager());

    if (!saveManager.applySaveToWorld(
            m_levelRuntime.getWorld(), *m_hudManager)) {
        return;
    }

    Hero* hero = m_levelRuntime.getHero();
    if (!hero) {
        std::cerr << "[PlayingState] ERROR: Save did not restore a Hero.\n";
        return;
    }

    // SaveManager replaces the Hero instance. Resolve its room before using
    // the room-dependent palette, music, camera or fall-death boundary.
    m_levelRuntime.syncActiveRegionToHero();

    const MapTheme activeTheme = m_levelRuntime.getWorld()
                                     .blockThemePalette()
                                     .getActiveTheme();
    game.getSoundManager().playBGM(getThemeBgm(activeTheme));

    // A loaded save starts a fresh attempt from the restored HUD snapshot.
    m_lastCoinCount = hero->getCoin();
    m_attemptStartScore = m_hudManager->getScore();
    m_attemptStartCoins = m_hudManager->getCoins();
    m_attemptStartLives = m_hudManager->getLives();
    m_latchedPipeDirection = PipeDirection::None;
    m_victoryPending = false;
    m_victoryDelayRemaining = 0.0f;
    m_defeatPending = false;
    m_defeatDelayRemaining = 0.0f;
    m_invincibilityBgmActive = false;

    const float halfScreenWidth = CAMERA_WIDTH * 0.5f;
    const float levelEnd = m_levelRuntime.getWorldWidth();
    const float cameraX = std::clamp(hero->getPosition().x,
                                     halfScreenWidth,
                                     levelEnd - halfScreenWidth);
    const float cameraY = getCameraY(
        m_levelRuntime.getActiveRegionBottom());
    m_camera.setCenter(std::round(cameraX), std::round(cameraY));
}
