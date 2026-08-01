#include "Core/PausedState.h"
#include "Core/Game.h"
#include <iostream>

PausedState::PausedState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "ERROR: Failed to load font!\n";
    }

    m_pauseText.setFont(m_font);
    m_pauseText.setString("PAUSED");
    m_pauseText.setCharacterSize(60);
    m_pauseText.setFillColor(sf::Color::White);

    m_resumeText.setFont(m_font);
    m_resumeText.setString("Press TAB to Resume");
    m_resumeText.setCharacterSize(24);
    m_resumeText.setFillColor(sf::Color::Yellow);
}

void PausedState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab) {
            std::cout << "RESUMING GAME...\n";
            Game::getInstance().popState();
        }
    }
}

void PausedState::update(sf::Time dt) {
    // Game is paused, no physics updates happen here.
}

void PausedState::render(sf::RenderWindow& window) {
    m_pauseText.setPosition((window.getSize().x - m_pauseText.getLocalBounds().width) / 2.f, 200.f);
    m_resumeText.setPosition((window.getSize().x - m_resumeText.getLocalBounds().width) / 2.f, 400.f);

    window.draw(m_pauseText);
    window.draw(m_resumeText);
}