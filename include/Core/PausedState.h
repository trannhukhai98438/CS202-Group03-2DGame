#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>

class PausedState : public State {
private:
    sf::Font m_font;
    sf::Text m_pauseText;
    sf::Text m_resumeText;

public:
    PausedState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
};