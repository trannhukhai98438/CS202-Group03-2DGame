#include "Managers/HUDManager.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

HUDManager::HUDManager() 
    : m_score(0), m_coins(0), m_lives(3), m_world("1-1"), m_remainingTime(300.0f) {
}

bool HUDManager::init(const std::string& fontPath) {
    if (!m_font.loadFromFile(fontPath)) {
        std::cerr << "[HUDManager] ERROR: cannot load font from " << fontPath << std::endl;
        return false;
    }

    // General formatting configuration for SFML text
    auto setupText = [this](sf::Text& text, float x, float y) {
        text.setFont(m_font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setPosition(x, y);
    };

    // Position the data labels in the top bar.
    setupText(m_scoreText, 60.0f,   20.0f);
    setupText(m_coinsText, 320.0f,  20.0f);
    setupText(m_worldText, 580.0f,  20.0f);
    setupText(m_timeText,  840.0f,  20.0f);
    setupText(m_livesText, 1060.0f, 20.0f);

    m_scoreText.setCharacterSize(24);
    m_coinsText.setCharacterSize(24);
    m_worldText.setCharacterSize(24);
    m_timeText.setCharacterSize(24);
    m_livesText.setCharacterSize(24);

    updateTextStrings();
    return true;
}

void HUDManager::updateTextStrings() {
    std::stringstream ssScore, ssCoins, ssWorld, ssTime, ssLives;

    ssScore << "MARIO\n" << std::setw(6) << std::setfill('0') << m_score;
    ssCoins << "COINS\n" << "x" << std::setw(2) << std::setfill('0') << m_coins;
    ssWorld << "WORLD\n " << m_world;
    ssTime  << "TIME\n " << std::setw(3) << std::setfill('0') << static_cast<int>(m_remainingTime);
    ssLives << "LIVES\n  " << m_lives;

    m_scoreText.setString(ssScore.str());
    m_coinsText.setString(ssCoins.str());
    m_worldText.setString(ssWorld.str());
    m_timeText.setString(ssTime.str());
    m_livesText.setString(ssLives.str());
}

void HUDManager::updateTimer(float dt) {
    if (m_remainingTime > 0) {
        m_remainingTime -= dt;
        if (m_remainingTime < 0) m_remainingTime = 0;
        updateTextStrings();
    }
}

void HUDManager::addScore(int points) {
    m_score += points;
    updateTextStrings();
}

void HUDManager::addCoin(int amount) {
    m_coins += amount;
    if (m_coins >= 100) {
        m_coins -= 100;
        m_lives++;
        m_score += 200;
    }
    updateTextStrings();
}

void HUDManager::setLives(int lives) {
    m_lives = lives;
    updateTextStrings();
}

void HUDManager::setWorld(std::string worldIndex) {
    m_world = worldIndex;
    updateTextStrings();
}

void HUDManager::resetTimer(float seconds) {
    m_remainingTime = seconds;
    updateTextStrings();
}

void HUDManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Note: Lock the HUD in place according to the camera view so that the HUD doesn't drift when the screen scrolls.
    sf::View currentView = target.getView();
    target.setView(target.getDefaultView());

    target.draw(m_scoreText, states);
    target.draw(m_coinsText, states);
    target.draw(m_worldText, states);
    target.draw(m_timeText, states);
    target.draw(m_livesText, states);

    // Restore the old View to Game World.
    target.setView(currentView);
}