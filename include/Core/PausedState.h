#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>
#include <array>

class PausedState : public State {
private:
    struct PauseButton {
        sf::RectangleShape shape;
        sf::Text label;
    };

    void selectButton(int index);
    void activateButton(int index);
    void updateButtonAppearance();
    int buttonAt(sf::Vector2f point) const;

    sf::Font m_font;
    sf::RectangleShape m_fadeOverlay;
    sf::Text m_pauseText;
    std::array<PauseButton, 2> m_buttons;
    int m_selectedButton{0};
    int m_hoveredButton{-1};

public:
    PausedState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
    bool rendersBelow() const override { return true; }
};
