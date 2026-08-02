#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include <iostream>

PlayingState::PlayingState() 
	: m_velocityX(0.f), m_velocityY(0.f) {
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }
    m_hero = HeroFactory::createHero(HeroType::Mario, 100.f, 500.f);

    m_dummyFloor.setSize(sf::Vector2f(1280.f, 120.f));
    m_dummyFloor.setFillColor(sf::Color::Green);
    m_dummyFloor.setPosition(0.f, 600.f);

    m_dummyWall.setSize(sf::Vector2f(50.f, 200.f));
    m_dummyWall.setFillColor(sf::Color::Yellow);
    m_dummyWall.setPosition(800.f, 400.f);

    auto spawnCallback = [this](std::unique_ptr<Projectile> p) {
        m_projectiles.push_back(std::move(p));
    };

    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Goomba, 300.f, 536.f, 150.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Koopa, 600.f, 504.f, 200.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Witch, 900.f, 504.f, 150.f, spawnCallback));
    MapObject spawnPoint;
    if (m_levelManager.getObjectByName("Objects", "SpawnPoint", spawnPoint)) {
        m_mario.setPosition(spawnPoint.x, spawnPoint.y);
    } else {
        m_mario.setPosition(100.f, 0.f);
    }
}

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
    
    // Simulate getting coins for UI testing
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        m_hudManager.addCoin(1);
        m_hudManager.addScore(100);
    }
}

void PlayingState::update(sf::Time dt) {
    m_hudManager.updateTimer(dt.asSeconds());
    if (m_hudManager.getRemainingTime() <= 0.0f) {
        // Time out
    }

    if (m_hero) {
        m_hero->update(dt.asSeconds());

        // Simple floor collision - only if alive and falling
        if (!m_hero->isDead() && m_hero->getPosition().y >= 600.f && m_hero->getVelocity().y > 0.f) {
            m_hero->setPosition(m_hero->getPosition().x, 600.f);
            m_hero->setVelocity(m_hero->getVelocity().x, 0.f);
            m_hero->setGrounded(true);
        }
    }

    // Update enemies
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        (*it)->update(dt.asSeconds());
        
        if ((*it)->getIsAlive() && (*it)->getStateName() != "FlippingDeath" && m_hero && !m_hero->isDead() && (*it)->getBounds().intersects(m_hero->getBounds())) {
            sf::FloatRect enemyBounds = (*it)->getBounds();
            sf::FloatRect heroBounds = m_hero->getBounds();
            
            // Stomp logic (falling down on top of enemy)
            bool isFalling = (m_hero->getVelocity().y >= 0.f);
            float marioCenterY = heroBounds.top + (heroBounds.height * 0.5f);
            float enemyBottomY = enemyBounds.top + enemyBounds.height;

            if (isFalling && marioCenterY < enemyBottomY) {
                (*it)->onStomped(nullptr);
                m_hero->setVelocity(m_hero->getVelocity().x, -600.f);
                m_hudManager.addScore(200); // Add score for stomping
            } else {
                (*it)->onSideCollision(nullptr);
                if ((*it)->getDamageOnTouch() > 0) {
                    m_hero->takedamage();
                }
            }
        }

        if (!(*it)->getIsAlive()) {
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        (*it)->update(dt.asSeconds());
        if ((*it)->getIsAlive() && m_hero && !m_hero->isDead() && (*it)->getBounds().intersects(m_hero->getBounds())) {
            (*it)->die();
            m_hero->takedamage();
        }
        if (!(*it)->getIsAlive()) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_levelManager);

    window.draw(m_dummyFloor);
    if (m_hero) {
        m_hero->render(window);
    }
    window.draw(m_dummyWall);

    for (auto& enemy : m_enemies) {
        if (enemy) {
            enemy->render(window);
        }
    }

    for (auto& projectile : m_projectiles) {
        if (projectile) {
            projectile->render(window);
        }
    }
    window.draw(m_hudManager);
	window.setView(window.getDefaultView()); // Reset view to default for UI rendering
}