#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include <iostream>

PlayingState::PlayingState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "ERROR: Failed to load font!\n";
    }

    m_placeholderText.setFont(m_font);
    m_placeholderText.setString("PLAYING STATE");
    m_placeholderText.setCharacterSize(40);
    m_placeholderText.setFillColor(sf::Color::White);
}

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "GAME PAUSED!\n";
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
}

void PlayingState::update(sf::Time dt) {
    // TODO: Mario's physics and enemy AI will update here
}

void PlayingState::render(sf::RenderWindow& window) {
    m_placeholderText.setPosition(
        (window.getSize().x - m_placeholderText.getLocalBounds().width) / 2.f,
        300.f
    );

    window.draw(m_placeholderText);
}