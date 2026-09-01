#include "Core/LevelSelectState.h"

#include "Core/Game.h"
#include "Core/LevelCatalog.h"
#include "Core/MainMenuState.h"

#include <array>
#include <iostream>
#include <memory>
#include <string>

namespace {
void centerText(sf::Text& text, sf::Vector2f position) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(position);
}
}

LevelSelectState::LevelSelectState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "Error loading level-select font\n";
    }
    if (!m_backgroundTexture.loadFromFile(
            "assets/textures/menuBackground.png")) {
        std::cerr << "Error loading level-select background\n";
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    if (backgroundSize.x > 0 && backgroundSize.y > 0) {
        m_backgroundSprite.setScale(1280.f / backgroundSize.x,
                                    720.f / backgroundSize.y);
    }

    m_overlay.setSize({1280.f, 720.f});
    m_overlay.setFillColor(sf::Color(0, 0, 0, 125));

    m_title.setFont(m_font);
    m_title.setString("CHOOSE A LEVEL");
    m_title.setCharacterSize(48);
    m_title.setFillColor(sf::Color(255, 220, 55));
    m_title.setOutlineColor(sf::Color(125, 20, 20));
    m_title.setOutlineThickness(4.f);
    centerText(m_title, {640.f, 85.f});

    const std::array<float, 3> xPositions = {280.f, 640.f, 1000.f};
    for (std::size_t i = 0; i < m_cards.size(); ++i) {
        LevelCard& card = m_cards[i];
        const LevelCatalog::LevelDefinition& option =
            LevelCatalog::LEVELS[i];

        card.panel.setSize({270.f, 330.f});
        card.panel.setOrigin(135.f, 165.f);
        card.panel.setPosition(xPositions[i], 325.f);
        card.panel.setOutlineThickness(5.f);

        card.number.setFont(m_font);
        card.number.setString(option.number);
        card.number.setCharacterSize(96);
        card.number.setStyle(sf::Text::Bold);
        centerText(card.number, {xPositions[i], 280.f});

        card.name.setFont(m_font);
        card.name.setString(option.name);
        card.name.setCharacterSize(27);
        centerText(card.name, {xPositions[i], 395.f});

        card.status.setFont(m_font);
        card.status.setString(option.status);
        card.status.setCharacterSize(i == 2 ? 13 : 16);
        centerText(card.status, {xPositions[i], 445.f});
    }

    m_confirmButton.setSize({270.f, 62.f});
    m_confirmButton.setOrigin(135.f, 31.f);
    m_confirmButton.setPosition(720.f, 580.f);
    m_confirmButton.setOutlineThickness(3.f);
    m_confirmText.setFont(m_font);
    m_confirmText.setString("SELECT");
    m_confirmText.setCharacterSize(25);
    centerText(m_confirmText, m_confirmButton.getPosition());

    m_backButton.setSize({190.f, 62.f});
    m_backButton.setOrigin(95.f, 31.f);
    m_backButton.setPosition(455.f, 580.f);
    m_backButton.setOutlineThickness(3.f);
    m_backText.setFont(m_font);
    m_backText.setString("BACK");
    m_backText.setCharacterSize(25);
    centerText(m_backText, m_backButton.getPosition());

    m_hint.setFont(m_font);
    m_hint.setString("CLICK A LEVEL OR USE LEFT / RIGHT");
    m_hint.setCharacterSize(15);
    m_hint.setFillColor(sf::Color(235, 235, 235));
    centerText(m_hint, {640.f, 665.f});

    const std::string& selectedPath =
        Game::getInstance().getSelectedLevelPath();
    for (std::size_t i = 0; i < LevelCatalog::LEVELS.size(); ++i) {
        if (selectedPath == LevelCatalog::LEVELS[i].mapPath) {
            m_selectedIndex = static_cast<int>(i);
            break;
        }
    }

    refreshAppearance();
}

void LevelSelectState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f point(static_cast<float>(event.mouseMove.x),
                                 static_cast<float>(event.mouseMove.y));
        m_hoveredCard = cardAt(point);
        m_confirmHovered = m_confirmButton.getGlobalBounds().contains(point);
        m_backHovered = m_backButton.getGlobalBounds().contains(point);
        refreshAppearance();
    }

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f point(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));
        const int clickedCard = cardAt(point);
        if (clickedCard >= 0) {
            m_selectedIndex = clickedCard;
            refreshAppearance();
        } else if (m_confirmButton.getGlobalBounds().contains(point)) {
            confirmSelection();
            return;
        } else if (m_backButton.getGlobalBounds().contains(point)) {
            Game::getInstance().changeState(
                std::make_unique<MainMenuState>());
            return;
        }
    }

    if (event.type != sf::Event::KeyPressed) return;

    if (event.key.code == sf::Keyboard::Left
        || event.key.code == sf::Keyboard::A) {
        m_selectedIndex = (m_selectedIndex + 2) % 3;
        refreshAppearance();
    } else if (event.key.code == sf::Keyboard::Right
               || event.key.code == sf::Keyboard::D) {
        m_selectedIndex = (m_selectedIndex + 1) % 3;
        refreshAppearance();
    } else if (event.key.code == sf::Keyboard::Enter) {
        confirmSelection();
    } else if (event.key.code == sf::Keyboard::Escape) {
        Game::getInstance().changeState(std::make_unique<MainMenuState>());
    }
}

void LevelSelectState::update(sf::Time dt) {
    (void)dt;
}

void LevelSelectState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
    window.draw(m_overlay);
    window.draw(m_title);
    for (const LevelCard& card : m_cards) {
        window.draw(card.panel);
        window.draw(card.number);
        window.draw(card.name);
        window.draw(card.status);
    }
    window.draw(m_backButton);
    window.draw(m_backText);
    window.draw(m_confirmButton);
    window.draw(m_confirmText);
    window.draw(m_hint);
}

int LevelSelectState::cardAt(sf::Vector2f point) const {
    for (std::size_t i = 0; i < m_cards.size(); ++i) {
        if (m_cards[i].panel.getGlobalBounds().contains(point)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void LevelSelectState::refreshAppearance() {
    for (std::size_t i = 0; i < m_cards.size(); ++i) {
        const bool selected = static_cast<int>(i) == m_selectedIndex;
        const bool hovered = static_cast<int>(i) == m_hoveredCard;
        LevelCard& card = m_cards[i];

        card.panel.setFillColor(selected
            ? sf::Color(255, 215, 70, 220)
            : hovered ? sf::Color(60, 85, 120, 225)
                      : sf::Color(20, 35, 65, 215));
        card.panel.setOutlineColor(selected
            ? sf::Color::White : sf::Color(180, 190, 210));

        const sf::Color labelColor = selected
            ? sf::Color(135, 25, 20) : sf::Color::White;
        card.number.setFillColor(labelColor);
        card.name.setFillColor(labelColor);
        card.status.setFillColor(i == 2
            ? (selected ? sf::Color(150, 35, 25)
                        : sf::Color(255, 190, 70))
            : (selected ? sf::Color(30, 105, 55)
                        : sf::Color(135, 235, 165)));
    }

    const auto styleButton = [](sf::RectangleShape& button,
                                sf::Text& label,
                                bool hovered) {
        button.setFillColor(hovered ? sf::Color(245, 195, 45, 235)
                                    : sf::Color(20, 35, 65, 220));
        button.setOutlineColor(sf::Color::White);
        label.setFillColor(hovered ? sf::Color(125, 25, 20)
                                   : sf::Color::White);
    };
    styleButton(m_confirmButton, m_confirmText, m_confirmHovered);
    styleButton(m_backButton, m_backText, m_backHovered);
}

void LevelSelectState::confirmSelection() {
    const LevelCatalog::LevelDefinition& selected =
        LevelCatalog::LEVELS[m_selectedIndex];
    Game::getInstance().setSelectedLevel(
        selected.mapPath, selected.worldName);
    Game::getInstance().changeState(std::make_unique<MainMenuState>());
}
