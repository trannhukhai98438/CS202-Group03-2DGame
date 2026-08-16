#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include <iostream>

PlayingState::PlayingState() {
    m_hudManager.init("assets/fonts/SuperMario256.ttf");

    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }

    m_soundManager.loadAllSFX();
    m_soundManager.playBGM("underground");

    // Initilize
    m_mario.setSize(sf::Vector2f(32.f, 32.f));
    m_mario.setFillColor(sf::Color::Red);

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
        m_soundManager.playSFX("coin");
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

    m_mario.move(m_velocity * dtSec);
}

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_levelManager);
    window.draw(m_mario);
    window.draw(m_hudManager);
	window.setView(window.getDefaultView()); // Reset view to default for UI rendering
}