#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <functional>

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
    sf::Text m_saveStatusText;
    std::array<PauseButton, 3> m_buttons;
    std::function<bool()> m_onSave;
    int m_selectedButton{0};
    int m_hoveredButton{-1};

public:
    explicit PausedState(std::function<bool()> onSave = {});

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
    bool rendersBelow() const override { return true; }
};
