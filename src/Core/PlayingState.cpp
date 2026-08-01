#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include <iostream>

PlayingState::PlayingState(): m_velocityX(200.f), m_velocityY(0.f){
        m_hudManager.init("assets/fonts/SuperMario256.ttf");

    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }
    m_dummyMario.setSize(sf::Vector2f(50.f, 50.f));
	m_dummyMario.setFillColor(sf::Color::White);
	m_dummyMario.setPosition(100.f, 550.f);
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

    // Bấm phím C để giả lập ăn 1 xu (+100 điểm)
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        m_hudManager.addCoin(1);
        m_hudManager.addScore(100);
    }
}

void PlayingState::update(sf::Time dt) {
    float dtSec = dt.asSeconds();

    sf::Vector2f nextPos = m_mario.getPosition();
    if (!m_levelManager.isSolidAtPixel(nextPos.x + 16.f, nextPos.y + 32.f)) {
        m_velocity.y += 980.f * dtSec;
    } else {
        m_velocity.y = 0.f;
    }

    m_dummyMario.move(m_velocity * dtSec);

    // Update enemies
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        (*it)->update(dt.asSeconds());
        
        if ((*it)->getIsAlive() && (*it)->getBounds().intersects(m_dummyMario.getGlobalBounds())) {
            (*it)->interactWith(nullptr);
        }

        if (!(*it)->getIsAlive()) {
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        (*it)->update(dt.asSeconds());
        if ((*it)->getIsAlive() && (*it)->getBounds().intersects(m_dummyMario.getGlobalBounds())) {
            (*it)->die();
            m_dummyMario.setFillColor(sf::Color::Red); // Visual feedback for hit
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