#include "Core/SettingsState.h"

#include "Core/Game.h"
#include "Core/MainMenuState.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

namespace {
constexpr float TRACK_LEFT = 420.0f;
constexpr float TRACK_WIDTH = 440.0f;
constexpr float KEYBOARD_VOLUME_STEP = 5.0f;

void centerText(sf::Text& text, sf::Vector2f position) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width * 0.5f,
                   bounds.top + bounds.height * 0.5f);
    text.setPosition(position);
}
}

SettingsState::SettingsState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "Error loading settings font\n";
    }
    if (!m_backgroundTexture.loadFromFile(
            "assets/textures/menuBackground.png")) {
        std::cerr << "Error loading settings background\n";
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    if (backgroundSize.x > 0 && backgroundSize.y > 0) {
        m_backgroundSprite.setScale(1280.0f / backgroundSize.x,
                                    720.0f / backgroundSize.y);
    }

    m_overlay.setSize({1280.0f, 720.0f});
    m_overlay.setFillColor(sf::Color(0, 0, 0, 125));

    m_panel.setSize({880.0f, 470.0f});
    m_panel.setPosition(200.0f, 135.0f);
    m_panel.setFillColor(sf::Color(12, 25, 48, 230));
    m_panel.setOutlineColor(sf::Color(235, 235, 235, 220));
    m_panel.setOutlineThickness(3.0f);

    m_title.setFont(m_font);
    m_title.setString("AUDIO SETTINGS");
    m_title.setCharacterSize(46);
    m_title.setFillColor(sf::Color(255, 220, 55));
    m_title.setOutlineColor(sf::Color(145, 25, 20));
    m_title.setOutlineThickness(3.0f);
    centerText(m_title, {640.0f, 92.0f});

    const std::array<const char*, 2> labels = {
        "THEME MUSIC", "SOUND EFFECTS"
    };
    const std::array<float, 2> trackY = {285.0f, 420.0f};
    for (std::size_t index = 0; index < m_sliders.size(); ++index) {
        VolumeSlider& slider = m_sliders[index];
        slider.label.setFont(m_font);
        slider.label.setString(labels[index]);
        slider.label.setCharacterSize(23);
        slider.label.setFillColor(sf::Color::White);
        centerText(slider.label, {640.0f, trackY[index] - 58.0f});

        slider.track.setSize({TRACK_WIDTH, 14.0f});
        slider.track.setPosition(TRACK_LEFT, trackY[index]);
        slider.track.setFillColor(sf::Color(55, 65, 85));
        slider.track.setOutlineColor(sf::Color(215, 220, 230));
        slider.track.setOutlineThickness(2.0f);

        slider.fill.setPosition(TRACK_LEFT, trackY[index]);
        slider.fill.setFillColor(sf::Color(245, 195, 45));

        slider.knob.setRadius(15.0f);
        slider.knob.setOrigin(15.0f, 15.0f);
        slider.knob.setOutlineThickness(3.0f);
        slider.knob.setPosition(TRACK_LEFT, trackY[index] + 7.0f);

        slider.valueText.setFont(m_font);
        slider.valueText.setCharacterSize(18);
        slider.valueText.setFillColor(sf::Color::White);
    }

    m_hint.setFont(m_font);
    m_hint.setString("DRAG A BAR OR USE ARROW KEYS");
    m_hint.setCharacterSize(16);
    m_hint.setFillColor(sf::Color(210, 215, 225));
    centerText(m_hint, {640.0f, 500.0f});

    m_backButton.setSize({250.0f, 56.0f});
    m_backButton.setOrigin(125.0f, 28.0f);
    m_backButton.setPosition(640.0f, 560.0f);
    m_backButton.setOutlineThickness(3.0f);
    m_backText.setFont(m_font);
    m_backText.setString("BACK");
    m_backText.setCharacterSize(22);
    centerText(m_backText, m_backButton.getPosition());

    Game& game = Game::getInstance();
    m_sliders[0].value = game.getThemeMusicVolume();
    m_sliders[1].value = game.getSfxVolume();
    refreshSlider(0);
    refreshSlider(1);
    refreshAppearance();

    m_previewSoundManager.setBGMVolume(m_sliders[0].value);
    m_previewSoundManager.setSFXVolume(m_sliders[1].value);
    m_previewSoundManager.loadAllSFX();
    m_previewSoundManager.playBGM("ground");
}

void SettingsState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::LostFocus) {
        m_draggingSlider = -1;
        return;
    }

    if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f point(static_cast<float>(event.mouseMove.x),
                                 static_cast<float>(event.mouseMove.y));
        if (m_draggingSlider >= 0) {
            updateSliderFromMouse(m_draggingSlider, point.x, false);
        }
        m_hoveredSlider = sliderAt(point);
        m_backHovered = m_backButton.getGlobalBounds().contains(point);
        if (m_hoveredSlider >= 0) {
            m_selectedControl = m_hoveredSlider;
        } else if (m_backHovered) {
            m_selectedControl = 2;
        }
        refreshAppearance();
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f point(static_cast<float>(event.mouseButton.x),
                                 static_cast<float>(event.mouseButton.y));
        const int sliderIndex = sliderAt(point);
        if (sliderIndex >= 0) {
            m_selectedControl = sliderIndex;
            m_draggingSlider = sliderIndex;
            updateSliderFromMouse(sliderIndex, point.x, false);
            refreshAppearance();
        } else if (m_backButton.getGlobalBounds().contains(point)) {
            returnToMenu();
        }
        return;
    }

    if (event.type == sf::Event::MouseButtonReleased
        && event.mouseButton.button == sf::Mouse::Left) {
        if (m_draggingSlider == 1) {
            m_previewSoundManager.playSFX("coin");
        }
        m_draggingSlider = -1;
        return;
    }

    if (event.type != sf::Event::KeyPressed) return;

    if (event.key.code == sf::Keyboard::Escape) {
        returnToMenu();
        return;
    }
    if (event.key.code == sf::Keyboard::Up) {
        selectControl((m_selectedControl + 2) % 3);
        return;
    }
    if (event.key.code == sf::Keyboard::Down) {
        selectControl((m_selectedControl + 1) % 3);
        return;
    }
    if ((event.key.code == sf::Keyboard::Left
         || event.key.code == sf::Keyboard::Right)
        && m_selectedControl < 2) {
        const float direction = event.key.code == sf::Keyboard::Left
            ? -1.0f : 1.0f;
        setSliderValue(
            m_selectedControl,
            m_sliders[m_selectedControl].value
                + direction * KEYBOARD_VOLUME_STEP,
            m_selectedControl == 1);
        return;
    }
    if (event.key.code == sf::Keyboard::Enter) {
        if (m_selectedControl == 2) {
            returnToMenu();
        } else if (m_selectedControl == 1) {
            m_previewSoundManager.playSFX("coin");
        }
    }
}

void SettingsState::update(sf::Time dt) {
    (void)dt;
}

void SettingsState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
    window.draw(m_overlay);
    window.draw(m_panel);
    window.draw(m_title);
    for (const VolumeSlider& slider : m_sliders) {
        window.draw(slider.label);
        window.draw(slider.track);
        window.draw(slider.fill);
        window.draw(slider.knob);
        window.draw(slider.valueText);
    }
    window.draw(m_hint);
    window.draw(m_backButton);
    window.draw(m_backText);
}

int SettingsState::sliderAt(sf::Vector2f point) const {
    for (std::size_t index = 0; index < m_sliders.size(); ++index) {
        const sf::FloatRect trackBounds =
            m_sliders[index].track.getGlobalBounds();
        const sf::FloatRect hitArea(trackBounds.left - 20.0f,
                                    trackBounds.top - 22.0f,
                                    trackBounds.width + 40.0f,
                                    trackBounds.height + 44.0f);
        if (hitArea.contains(point)) return static_cast<int>(index);
    }
    return -1;
}

void SettingsState::updateSliderFromMouse(int index, float mouseX,
                                          bool previewSfx) {
    const float ratio = std::clamp(
        (mouseX - TRACK_LEFT) / TRACK_WIDTH, 0.0f, 1.0f);
    setSliderValue(index, std::round(ratio * 100.0f), previewSfx);
}

void SettingsState::setSliderValue(int index, float value, bool previewSfx) {
    if (index < 0 || index >= static_cast<int>(m_sliders.size())) return;

    VolumeSlider& slider = m_sliders[index];
    slider.value = std::clamp(std::round(value), 0.0f, 100.0f);
    Game& game = Game::getInstance();
    if (index == 0) {
        game.setThemeMusicVolume(slider.value);
        m_previewSoundManager.setBGMVolume(slider.value);
    } else {
        game.setSfxVolume(slider.value);
        m_previewSoundManager.setSFXVolume(slider.value);
    }
    refreshSlider(index);
    if (previewSfx && index == 1) {
        m_previewSoundManager.playSFX("coin");
    }
}

void SettingsState::refreshSlider(int index) {
    VolumeSlider& slider = m_sliders[index];
    const float ratio = slider.value / 100.0f;
    slider.fill.setSize({TRACK_WIDTH * ratio, 14.0f});
    slider.knob.setPosition(TRACK_LEFT + TRACK_WIDTH * ratio,
                            slider.track.getPosition().y + 7.0f);
    slider.valueText.setString(slider.value <= 0.0f
        ? "MUTED"
        : std::to_string(static_cast<int>(slider.value)) + "%");
    centerText(slider.valueText,
               {930.0f, slider.track.getPosition().y + 7.0f});
}

void SettingsState::refreshAppearance() {
    for (std::size_t index = 0; index < m_sliders.size(); ++index) {
        const bool active = static_cast<int>(index) == m_selectedControl
            || static_cast<int>(index) == m_hoveredSlider
            || static_cast<int>(index) == m_draggingSlider;
        m_sliders[index].knob.setFillColor(active
            ? sf::Color(245, 195, 45) : sf::Color::White);
        m_sliders[index].knob.setOutlineColor(active
            ? sf::Color::White : sf::Color(130, 140, 160));
        m_sliders[index].label.setFillColor(active
            ? sf::Color(255, 220, 55) : sf::Color::White);
    }

    const bool backActive = m_selectedControl == 2 || m_backHovered;
    m_backButton.setFillColor(backActive
        ? sf::Color(245, 195, 45, 235)
        : sf::Color(25, 55, 90, 230));
    m_backButton.setOutlineColor(sf::Color::White);
    m_backText.setFillColor(backActive
        ? sf::Color(125, 25, 20) : sf::Color::White);
}

void SettingsState::selectControl(int index) {
    m_selectedControl = std::clamp(index, 0, 2);
    m_hoveredSlider = -1;
    m_backHovered = false;
    refreshAppearance();
}

void SettingsState::returnToMenu() {
    Game::getInstance().changeState(std::make_unique<MainMenuState>());
}
