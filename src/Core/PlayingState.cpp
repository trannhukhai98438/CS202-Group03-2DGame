#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"

PlayingState::PlayingState(): m_velocityX(200.f), m_velocityY(0.f){
    m_hudManager.init("assets/fonts/SuperMario256.ttf");

	// TODO (Khai): Replace these dummy shapes with actual Mario and level assets when available.
    m_camera.setSize(1280.f, 720.f);
    m_dummyMario.setSize(sf::Vector2f(50.f, 50.f));
    m_dummyMario.setFillColor(sf::Color::Cyan);
    m_dummyMario.setPosition(600.f, 100.f);

    m_dummyFloor.setSize(sf::Vector2f(5000.f, 50.f));
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

    // Bấm phím C để giả lập ăn 1 xu (+100 điểm)
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        m_hudManager.addCoin(1);
        m_hudManager.addScore(100);
    }
}

void PlayingState::update(sf::Time dt) {
    m_hudManager.updateTimer(dt.asSeconds());
    if (m_hudManager.getRemainingTime() <= 0.0f) {
        // Time out --> Game Over
        // Game::getInstance().changeState(std::make_unique<GameOverState>());
    }

	// Temporary (Delete this when we have a proper Mario sprite and level assets)
    float dummyVelocityX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        dummyVelocityX = 300.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        dummyVelocityX = -300.f;
    }
    // X-axis movement
    m_dummyMario.move(dummyVelocityX * dt.asSeconds(), 0.f);
    m_physics.resolveCollisionX(m_dummyMario, m_dummyFloor, dummyVelocityX);
    m_physics.resolveCollisionX(m_dummyMario, m_dummyWall, dummyVelocityX);
    // Y-axis movement
    m_physics.applyGravity(m_velocityY, dt.asSeconds());
    m_dummyMario.move(0.f, m_velocityY * dt.asSeconds());
    m_physics.resolveCollisionY(m_dummyMario, m_dummyFloor, m_velocityY);
    m_physics.resolveCollisionY(m_dummyMario, m_dummyWall, m_velocityY);

    float marioX = m_dummyMario.getPosition().x;
    float halfScreenWidth = 640.f;
    float levelEnd = 5000.f; //Wherever the level ends. This is just a PLACEHOLDER for now.
    float cameraX = std::clamp(marioX, halfScreenWidth, levelEnd - halfScreenWidth);
    m_camera.setCenter(cameraX, 360.f);
	// TEST SCREENS (delete this when we have a proper Mario sprite and level assets)
    // Press 'L' to simulate Mario dying
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
        Game::getInstance().changeState(std::make_unique<GameOverState>());
    }

    // Press 'W' to simulate touching the flagpole
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        Game::getInstance().changeState(std::make_unique<VictoryState>());
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.setView(m_camera);
    window.draw(m_dummyFloor);
    window.draw(m_dummyMario);
    window.draw(m_dummyWall);

    window.draw(m_hudManager);
	window.setView(window.getDefaultView()); // Reset view to default for UI rendering
}