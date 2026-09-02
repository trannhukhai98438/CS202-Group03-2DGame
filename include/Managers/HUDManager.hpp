#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// The HUDManager class is responsible for displaying parameters and updating them based on events. (Observer pattern)
class HUDManager : public sf::Drawable {
private:
    sf::Font m_font;
    sf::Text m_scoreText;
    sf::Text m_coinsText;
    sf::Text m_worldText;
    sf::Text m_timeText;
    sf::Text m_livesText;

    int m_score;
    int m_coins;
    int m_lives;
    std::string m_world;
    float m_remainingTime;

    void updateTextStrings();

    // Internal drawing function from sf::Drawable
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    HUDManager();
    bool init(const std::string& fontPath);

    // Updated countdown timer in Game Loop (delta time)
    void updateTimer(float dt);

    // --- Observer functions (Called when Game Data changes) ---
    void addScore(int points);
    void addCoin(int amount = 1);
    void setLives(int lives);
    void restoreProgress(int score, int coins, int lives);
    int loseLife();
    void setWorld(std::string worldIndex);
    void resetTimer(float seconds = 300.0f);

    // Getters
    int getScore() const { return m_score; }
    int getCoins() const { return m_coins; }
    int getLives() const { return m_lives; }
    float getRemainingTime() const { return m_remainingTime; }
};
