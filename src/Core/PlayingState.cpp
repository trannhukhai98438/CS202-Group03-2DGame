#include "Core/PlayingState.h"

#include "Core/Game.h"
#include "Core/GameOverState.h"
#include "Core/PausedState.h"
#include "Core/TransitionState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroForm/FireForm.h"
#include "Entities/Character/Hero/HeroForm/GiantForm.h"
#include "Entities/Character/Hero/HeroForm/SmallForm.h"
#include "Entities/Character/Hero/HeroState/CheerState.h"
#include "Managers/SaveManager.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

namespace {
constexpr float VICTORY_DELAY_SECONDS = 0.75f;
constexpr float CAMERA_WIDTH = 1280.0f;
constexpr float CAMERA_HEIGHT = 720.0f;

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

HeroType parseHeroType(const std::string& heroType) {
    if (heroType == "Luigi") return HeroType::Luigi;
    if (heroType == "Flash") return HeroType::Flash;
    return HeroType::Mario;
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

std::string getWorldName(const std::string& levelPath) {
    const std::size_t fileStart = levelPath.find_last_of("/\\");
    const std::size_t nameStart = fileStart == std::string::npos
        ? 0 : fileStart + 1;
    const std::size_t extensionStart = levelPath.find_last_of('.');
    const std::size_t nameLength = extensionStart == std::string::npos
        || extensionStart < nameStart
            ? std::string::npos
            : extensionStart - nameStart;
    return "WORLD " + levelPath.substr(nameStart, nameLength);
}
}

PlayingState::PlayingState(std::shared_ptr<HUDManager> hudManager, const std::string& levelPath)
	: m_hudManager(std::move(hudManager)),
	  m_levelPath(levelPath),
	  m_levelRuntime(levelPath,
	                 "assets/maps/resources/tileset.png",
	                 Game::getInstance().getSelectedHero()) {
    m_vfxTexture.loadFromFile("assets/textures/boss_vfx.png");
    std::cout << "[PlayingState] Loaded map: " << levelPath << std::endl;
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
    syncRegionPresentation(true);
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
        if (event.key.code == sf::Keyboard::Tab) {
            Game::getInstance().getSoundManager().playSFX("pause");
            Game::getInstance().pushState(std::make_unique<PausedState>());
            return;
        }

        if (event.key.code == sf::Keyboard::F5) {
            quickSave();
            return;
        }
        if (event.key.code == sf::Keyboard::F9) {
            quickLoad();
            return;
        }

        // ============================================================
        // DEBUG TEST HOTKEYS (Instant Phase Switch & Testing)
        // ============================================================
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::F1) {
            for (auto& enemy : m_levelRuntime.getWorld().enemies()) {
                if (auto* boss = dynamic_cast<ThorKing*>(enemy.get())) {
                    boss->setBossHp(3);
                    std::cout << "[DEBUG] Switched ThorKing Boss to Phase 1 (HP 3/3)" << std::endl;
                }
            }
        } else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::F2) {
            for (auto& enemy : m_levelRuntime.getWorld().enemies()) {
                if (auto* boss = dynamic_cast<ThorKing*>(enemy.get())) {
                    boss->setBossHp(2);
                    std::cout << "[DEBUG] Switched ThorKing Boss to Phase 2 (HP 2/3)" << std::endl;
                }
            }
        } else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::F3) {
            for (auto& enemy : m_levelRuntime.getWorld().enemies()) {
                if (auto* boss = dynamic_cast<ThorKing*>(enemy.get())) {
                    boss->setBossHp(1);
                    std::cout << "[DEBUG] Switched ThorKing Boss to Phase 3 (HP 1/3 - Magma Winged Demon)" << std::endl;
                }
            }
        } else if (event.key.code == sf::Keyboard::F || event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::F4) {
            if (auto* hero = m_levelRuntime.getHero()) {
                hero->setForm(std::make_unique<FireForm>());
                std::cout << "[DEBUG] Switched Hero to Fire Form (Press X to shoot fireball!)" << std::endl;
            }
        } else if (event.key.code == sf::Keyboard::G || event.key.code == sf::Keyboard::Num5 || event.key.code == sf::Keyboard::F6) {
            if (auto* hero = m_levelRuntime.getHero()) {
                hero->setForm(std::make_unique<GiantForm>());
                std::cout << "[DEBUG] Switched Hero to Giant Form" << std::endl;
            }
        } else if (event.key.code == sf::Keyboard::H || event.key.code == sf::Keyboard::Num6 || event.key.code == sf::Keyboard::F7) {
            if (auto* hero = m_levelRuntime.getHero()) {
                hero->setForm(std::make_unique<SmallForm>());
                std::cout << "[DEBUG] Switched Hero to Small Form" << std::endl;
            }
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
    syncRegionPresentation();
    if (!m_defeatPending && levelUpdate.scoreDelta > 0) {
        m_hudManager->addScore(levelUpdate.scoreDelta);
    }
    if (levelUpdate.travelledThroughPipe) {
        m_latchedPipeDirection = requestedPipeDirection;
        Game::getInstance().getSoundManager().playSFX("pipe");
    }

    const bool starmanActive = hero->getIsStarman() && hero->getInvincibleTimer() > 0.0f;
    if (starmanActive && !m_invincibilityBgmActive) {
        Game::getInstance().getSoundManager().stopBGM();
        Game::getInstance().getSoundManager().playBGM("invincibility", true);
        m_invincibilityBgmActive = true;
    } else if (!starmanActive && m_invincibilityBgmActive) {
        m_invincibilityBgmActive = false;
        syncRegionPresentation(true);
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
                std::make_unique<VictoryState>(
                    heroScreenPosition, m_hudManager));
            return;
        }
    }

    // Update Boss Environmental VFX for Phase 3
    m_vfxAnimTimer += deltaTime;
    bool isBossPhase3 = false;
    for (const auto& enemy : m_levelRuntime.getWorld().enemies()) {
        ThorKing* boss = dynamic_cast<ThorKing*>(enemy.get());
        if (boss && boss->getIsAlive() && (boss->getPhase() == 3 || boss->getStateName() == "TKRoar")) {
            isBossPhase3 = true;
            break;
        }
    }

    if (isBossPhase3) {
        const float regionBottom = m_levelRuntime.getActiveRegionBottom();
        const float screenTopY   = regionBottom - CAMERA_HEIGHT;

        // Get the actual floor Y from the boss's feet - 100% accurate
        float floorY = regionBottom - 64.f; // safe fallback
        for (const auto& enemy : m_levelRuntime.getWorld().enemies()) {
            ThorKing* b = dynamic_cast<ThorKing*>(enemy.get());
            if (b && b->getIsAlive()) {
                floorY = b->getPosition().y + b->getHitbox().getSize().y;
                break;
            }
        }
        // Shift visual floor Y up by 14px so impact is on the top surface of the grass/dirt tile
        const float visualFloorY = floorY - 14.f;

        m_lightningStrikeTimer -= deltaTime;
        if (m_lightningStrikeTimer <= 0.f) {
            m_lightningActive = true;
            m_lightningDamagedHero = false;
            m_lightningFrame = 0;
            m_lightningFrameTimer = 0.045f;
            m_lightningX = 150.0f + static_cast<float>(std::rand() % 1000);
            m_lightningStrikeTimer = 2.0f + static_cast<float>(std::rand() % 15) * 0.1f;
        }
        if (m_lightningActive) {
            m_lightningFrameTimer -= deltaTime;
            if (m_lightningFrameTimer <= 0.f) {
                m_lightningFrameTimer = 0.045f;
                m_lightningFrame++;

                // Spawn crater at visual floor Y - guaranteed to sit on the real ground
                if (m_lightningFrame == 2) {
                    m_groundCraters.push_back({m_lightningX + 20.f, visualFloorY, 2.5f, 2.5f, 0.f});
                }

                if (m_lightningFrame >= 8) {
                    m_lightningActive = false;
                }
            }

            // Deal damage to Mario if touched by the lightning bolt
            if (!m_lightningDamagedHero && m_lightningFrame >= 1 && m_lightningFrame <= 5) {
                sf::FloatRect lightningHitbox(m_lightningX + 5.f, screenTopY, 60.f, visualFloorY - screenTopY);
                if (hero->getBounds().intersects(lightningHitbox)) {
                    hero->takeDamage(1);
                    m_lightningDamagedHero = true;
                }
            }
        }
    } else {
        m_lightningActive = false;
    }

    // Update ground craters (advance animation and fade over time)
    for (auto& crater : m_groundCraters) {
        crater.animTimer += deltaTime;
        crater.remainingTime -= deltaTime;
    }
    m_groundCraters.erase(
        std::remove_if(m_groundCraters.begin(), m_groundCraters.end(),
            [](const GroundCrater& c) { return c.remainingTime <= 0.0f; }),
        m_groundCraters.end());

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
					std::make_unique<TransitionState>(
                        m_hudManager, m_levelPath,
                        getWorldName(m_levelPath)));
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
    const MapTheme activeTheme = m_levelRuntime.getWorld()
                                     .blockThemePalette()
                                     .getActiveTheme();
    if (activeTheme == MapTheme::Underground) {
        sf::RectangleShape undergroundBackdrop(m_camera.getSize());
        undergroundBackdrop.setPosition(
            m_camera.getCenter() - m_camera.getSize() * 0.5f);
        undergroundBackdrop.setFillColor(sf::Color::Black);
        window.draw(undergroundBackdrop);
    }
    m_levelRuntime.renderWorld(window);

    // Render Environmental Cataclysm VFX (Dark Sky, Lightning, Levitating Debris)
    for (const auto& enemy : m_levelRuntime.getWorld().enemies()) {
        ThorKing* boss = dynamic_cast<ThorKing*>(enemy.get());
        if (boss && boss->getIsAlive()) {
            if (boss->getPhase() == 3 || boss->getStateName() == "TKRoar") {
                // 1. Dark Apocalyptic Sky Atmosphere
                sf::RectangleShape darkOverlay(sf::Vector2f(m_camera.getSize().x * 2.5f, m_camera.getSize().y * 2.5f));
                darkOverlay.setPosition(m_camera.getCenter().x - m_camera.getSize().x * 1.25f, m_camera.getCenter().y - m_camera.getSize().y * 1.25f);
                darkOverlay.setFillColor(sf::Color(15, 8, 25, 110)); // Deep atmospheric dark tint
                window.draw(darkOverlay);

                if (m_vfxTexture.getSize().x > 0) {
                    const float regionBottom = m_levelRuntime.getActiveRegionBottom();
                    const float screenTopY   = regionBottom - CAMERA_HEIGHT;

                    // Actual floor surface = boss feet Y (100% accurate, no tile-size guessing)
                    float floorY = regionBottom - 64.f; // safe fallback
                    floorY = boss->getPosition().y + boss->getHitbox().getSize().y;
                    const float visualFloorY = floorY - 14.f;

                    // 2. Red Lightning Strike — from screen top DOWN TO floor surface (not into ground)
                    if (m_lightningActive && m_lightningFrame < 8) {
                        static const sf::IntRect lightningRects[8] = {
                            sf::IntRect(25, 151, 86, 126),
                            sf::IntRect(146, 151, 98, 126),
                            sf::IntRect(276, 151, 89, 126),
                            sf::IntRect(416, 151, 82, 126),
                            sf::IntRect(544, 151, 63, 126),
                            sf::IntRect(678, 151, 54, 126),
                            sf::IntRect(808, 151, 46, 126),
                            sf::IntRect(936, 151, 37, 126)
                        };
                        const auto& lr = lightningRects[m_lightningFrame];
                        sf::Sprite lightningSprite(m_vfxTexture, lr);
                        float scaleX = 80.f / static_cast<float>(lr.width);
                        // Scale Y so bolt tip ends exactly at visualFloorY
                        float scaleY = (visualFloorY - screenTopY) / static_cast<float>(lr.height);
                        lightningSprite.setScale(scaleX, scaleY);
                        lightningSprite.setPosition(m_lightningX, screenTopY);
                        window.draw(lightningSprite);
                    }

                    // 3. Ground Craters — centered on floor surface
                    static const sf::IntRect craterRects[6] = {
                        sf::IntRect(32, 293, 106, 120),
                        sf::IntRect(192, 293, 132, 120),
                        sf::IntRect(350, 293, 152, 120),
                        sf::IntRect(521, 293, 149, 120),
                        sf::IntRect(687, 293, 155, 120),
                        sf::IntRect(857, 293, 157, 120)
                    };
                    for (const auto& crater : m_groundCraters) {
                        int cFrame = static_cast<int>(crater.animTimer * 10.0f);
                        if (cFrame >= 6) cFrame = 5;
                        sf::Sprite craterSprite(m_vfxTexture, craterRects[cFrame]);
                        // Set origin slightly higher (0.45f of height) to pull the visual crack up
                        craterSprite.setOrigin(craterRects[cFrame].width / 2.0f,
                                               craterRects[cFrame].height * 0.45f);
                        craterSprite.setScale(0.85f, 0.85f);
                        craterSprite.setPosition(crater.x, crater.y); // crater.y = visualFloorY

                        float alphaFactor = std::clamp(crater.remainingTime / crater.totalTime, 0.0f, 1.0f);
                        sf::Uint8 alpha = static_cast<sf::Uint8>(alphaFactor * 255.0f);
                        craterSprite.setColor(sf::Color(255, 255, 255, alpha));
                        window.draw(craterSprite);
                    }

                    // Ground Fissure & Rising Rock Debris around Boss during transformation roar
                    if (boss->getStateName() == "TKRoar") {
                        float bossCenterX = boss->getPosition().x + boss->getHitbox().getSize().x / 2.0f;
                        // Use actual floor Y: boss feet when standing on ground
                        float floorY = boss->getPosition().y + boss->getHitbox().getSize().y;

                        // 1. Ground Fissure under Boss — snapped to floor
                        int fissureFrame = static_cast<int>(m_vfxAnimTimer * 9.0f) % 6;
                        static const sf::IntRect fissureRects[6] = {
                            sf::IntRect(32, 293, 106, 120),
                            sf::IntRect(192, 293, 132, 120),
                            sf::IntRect(350, 293, 152, 120),
                            sf::IntRect(521, 293, 149, 120),
                            sf::IntRect(687, 293, 155, 120),
                            sf::IntRect(857, 293, 157, 120)
                        };
                        sf::Sprite fissureSprite(m_vfxTexture, fissureRects[fissureFrame]);
                        fissureSprite.setOrigin(
                            fissureRects[fissureFrame].width / 2.0f,
                            static_cast<float>(
                                fissureRects[fissureFrame].height));
                        fissureSprite.setPosition(bossCenterX, floorY);
                        window.draw(fissureSprite);

                        // 2. Rising Volcanic Rock Debris — anchored at floor, rising upward
                        int debrisFrame = static_cast<int>(m_vfxAnimTimer * 8.0f) % 7;
                        static const sf::IntRect debrisRects[7] = {
                            sf::IntRect(18, 16, 93, 119),
                            sf::IntRect(142, 16, 97, 119),
                            sf::IntRect(267, 16, 104, 119),
                            sf::IntRect(392, 16, 108, 119),
                            sf::IntRect(518, 16, 110, 119),
                            sf::IntRect(644, 16, 112, 119),
                            sf::IntRect(790, 16, 93, 119)
                        };
                        sf::Sprite debrisLeft(m_vfxTexture, debrisRects[debrisFrame]);
                        debrisLeft.setOrigin(
                            debrisRects[debrisFrame].width / 2.0f,
                            static_cast<float>(
                                debrisRects[debrisFrame].height));
                        debrisLeft.setScale(0.7f, 0.7f);
                        debrisLeft.setPosition(bossCenterX - 55.f, floorY);
                        window.draw(debrisLeft);

                        sf::Sprite debrisRight(m_vfxTexture, debrisRects[(debrisFrame + 3) % 7]);
                        debrisRight.setOrigin(
                            debrisRects[(debrisFrame + 3) % 7].width / 2.0f,
                            static_cast<float>(
                                debrisRects[(debrisFrame + 3) % 7].height));
                        debrisRight.setScale(-0.7f, 0.7f);
                        debrisRight.setPosition(bossCenterX + 55.f, floorY);
                        window.draw(debrisRight);
                    }
                }
            }
            break;
        }
    }

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

    const SaveData& saveData = saveManager.getSaveData();
    if (saveData.mapPath.empty() || saveData.tilesetPath.empty()) {
        std::cerr << "[PlayingState] ERROR: Save has no valid level paths.\n";
        return;
    }

    const HeroType savedHeroType = parseHeroType(saveData.hero.heroType);
    Game::getInstance().setSelectedHero(savedHeroType);
    Game::getInstance().setSelectedLevel(
        saveData.mapPath, getWorldName(saveData.mapPath));
    m_levelPath = saveData.mapPath;

    m_levelRuntime.reload(
        saveData.mapPath,
        saveData.tilesetPath,
        savedHeroType
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
    syncRegionPresentation(true);

    const float halfScreenWidth = CAMERA_WIDTH * 0.5f;
    const float levelEnd = m_levelRuntime.getWorldWidth();
    const float cameraX = std::clamp(hero->getPosition().x,
                                     halfScreenWidth,
                                     levelEnd - halfScreenWidth);
    const float cameraY = getCameraY(
        m_levelRuntime.getActiveRegionBottom());
    m_camera.setCenter(std::round(cameraX), std::round(cameraY));
}

void PlayingState::syncRegionPresentation(bool forcePlayback) {
    const MapTheme activeTheme = m_levelRuntime.getWorld()
                                     .blockThemePalette()
                                     .getActiveTheme();
    const bool themeChanged = activeTheme != m_presentedTheme;

    m_presentedTheme = activeTheme;
    if ((!themeChanged && !forcePlayback) || m_invincibilityBgmActive
        || m_victoryPending || m_defeatPending) {
        return;
    }

    Game::getInstance().getSoundManager().playBGM(
        getThemeBgm(activeTheme), true);
}
