#pragma once

#include "Core/State.h"

#include <SFML/Graphics.hpp>
#include <array>

class GuideState : public State {
public:
    GuideState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    struct GuideButton {
        sf::RectangleShape shape;
        sf::Text label;
    };

    void showPage(int page);
    void updateButtonAppearance();
    int buttonAt(sf::Vector2f point) const;
    void returnToMenu();

    sf::Font m_font;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::RectangleShape m_overlay;
    sf::RectangleShape m_panel;

    sf::Text m_title;
    sf::Text m_pageTitle;
    sf::Text m_body;
    sf::Text m_pageIndicator;

    sf::Texture m_marioTexture;
    sf::Texture m_luigiTexture;
	sf::Texture m_flashTexture;
    sf::Texture m_mushroomTexture;
    sf::Texture m_flowerTexture;
    sf::Texture m_starTexture;
    sf::Texture m_coinTexture;
	sf::Texture m_keyboardTexture;
    std::array<sf::Sprite, 4> m_demoSprites;
    std::array<sf::Text, 4> m_demoLabels;
	std::array<sf::Sprite, 10> m_keyboardSprites;
	std::array<sf::Text, 6> m_controlDescriptions;
	std::array<sf::Text, 4> m_orLabels;
    std::array<GuideButton, 3> m_buttons;

    int m_currentPage{0};
    int m_hoveredButton{-1};
};
