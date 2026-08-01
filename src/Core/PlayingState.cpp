#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"

PlayingState::PlayingState() 
	: m_velocityX(0.f), m_velocityY(0.f) {
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
}

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        m_velocityY = -600.f;
    }
}

void PlayingState::update(sf::Time dt) {
    // X-axis movement
    m_dummyMario.move(m_velocityX * dt.asSeconds(), 0.f);
    m_physics.resolveCollisionX(m_dummyMario, m_dummyFloor, m_velocityX);
    m_physics.resolveCollisionX(m_dummyMario, m_dummyWall, m_velocityX);
    // Y-axis movement
    m_physics.applyGravity(m_velocityY, dt.asSeconds());
    m_dummyMario.move(0.f, m_velocityY * dt.asSeconds());
    m_physics.resolveCollisionY(m_dummyMario, m_dummyFloor, m_velocityY);
    m_physics.resolveCollisionY(m_dummyMario, m_dummyWall, m_velocityY);

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
    window.draw(m_dummyFloor);
    window.draw(m_dummyMario);
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
}