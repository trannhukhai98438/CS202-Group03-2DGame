#pragma once

#include "Core/State.h"

#include <SFML/Graphics.hpp>
#include <array>

class SettingsState : public State {
public:
    SettingsState();

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    struct VolumeSlider {
        sf::Text label;
        sf::RectangleShape track;
        sf::RectangleShape fill;
        sf::CircleShape knob;
        sf::Text valueText;
        float value{100.0f};
    };

    int sliderAt(sf::Vector2f point) const;
    void updateSliderFromMouse(int index, float mouseX, bool previewSfx);
    void setSliderValue(int index, float value, bool previewSfx);
    void refreshSlider(int index);
    void refreshAppearance();
    void selectControl(int index);
    void returnToMenu();

    sf::Font m_font;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::RectangleShape m_overlay;
    sf::RectangleShape m_panel;
    sf::Text m_title;
    sf::Text m_hint;
    std::array<VolumeSlider, 2> m_sliders;
    sf::RectangleShape m_backButton;
    sf::Text m_backText;

    int m_selectedControl{0};
    int m_hoveredSlider{-1};
    int m_draggingSlider{-1};
    bool m_backHovered{false};
};
