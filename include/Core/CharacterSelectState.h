#pragma once

#include "Core/State.h"
#include "Entities/Character/Hero/HeroFactory.h"

#include <SFML/Graphics.hpp>
#include <array>

class CharacterSelectState : public State {
public:
    CharacterSelectState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    struct CharacterCard {
        sf::RectangleShape panel;
        sf::Sprite sprite;
        sf::Text name;
    };

    int cardAt(sf::Vector2f point) const;
    void refreshAppearance();
    void confirmSelection();

    sf::Font m_font;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::RectangleShape m_overlay;
    sf::Texture m_marioTexture;
    sf::Texture m_luigiTexture;
    sf::Text m_title;
    sf::Text m_hint;
    std::array<CharacterCard, 2> m_cards;
    sf::RectangleShape m_confirmButton;
    sf::Text m_confirmText;
    sf::RectangleShape m_backButton;
    sf::Text m_backText;
    int m_selectedIndex{0};
    int m_hoveredCard{-1};
    bool m_confirmHovered{false};
    bool m_backHovered{false};
};
