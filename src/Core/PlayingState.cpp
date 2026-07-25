#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"

PlayingState::PlayingState(): m_velocityX(200.f), m_velocityY(0.f){
	// TODO (Khai): Replace these dummy shapes with actual Mario and level assets when available.
    m_dummyMario.setSize(sf::Vector2f(50.f, 50.f));
    m_dummyMario.setFillColor(sf::Color::Cyan);
    m_dummyMario.setPosition(600.f, 100.f);

    m_dummyFloor.setSize(sf::Vector2f(800.f, 50.f));
    m_dummyFloor.setFillColor(sf::Color::Green);
    m_dummyFloor.setPosition(240.f, 600.f);

    m_dummyWall.setSize(sf::Vector2f(50.f, 200.f));
    m_dummyWall.setFillColor(sf::Color::Magenta);
    m_dummyWall.setPosition(800.f, 400.f);
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
}

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_dummyFloor);
    window.draw(m_dummyMario);
    window.draw(m_dummyWall);
}