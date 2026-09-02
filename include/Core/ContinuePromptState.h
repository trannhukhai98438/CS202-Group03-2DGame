#pragma once

#include "Core/State.h"
#include "Managers/SaveManager.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <string>

class ContinuePromptState final : public State {
private:
    struct PromptButton {
        sf::RectangleShape shape;
        sf::Text label;
        bool enabled{true};
    };

    void selectButton(int index);
    void moveSelection(int direction);
    void activateButton(int index);
    void updateButtonAppearance();
    int buttonAt(sf::Vector2f point) const;

    void continueSavedProgress();
    void startNewGame();

    sf::Font m_font;
    sf::RectangleShape m_fadeOverlay;
    sf::RectangleShape m_panel;
    sf::Text m_titleText;
    sf::Text m_promptText;
    std::array<PromptButton, 2> m_buttons;

    SaveManager m_saveManager;
    bool m_saveAvailable{false};
    std::string m_savedMapPath;
    std::string m_savedWorldName;
    std::string m_savedHudWorldName;
    int m_selectedButton{0};
    int m_hoveredButton{-1};

public:
    ContinuePromptState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;
    bool rendersBelow() const override { return true; }
};
