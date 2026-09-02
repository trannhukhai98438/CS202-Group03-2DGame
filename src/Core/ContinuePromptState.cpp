#include "Core/ContinuePromptState.h"

#include "Core/Game.h"
#include "Core/LevelCatalog.h"
#include "Core/TransitionState.h"
#include "Managers/HUDManager.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace {
HeroType parseHeroType(const std::string& heroType) {
    if (heroType == "Luigi") return HeroType::Luigi;
    if (heroType == "Flash") return HeroType::Flash;
    return HeroType::Mario;
}

std::string normalizedPath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

bool pathExists(const std::string& path) {
    if (path.empty()) return false;
    std::error_code error;
    return std::filesystem::is_regular_file(path, error) && !error;
}
}

ContinuePromptState::ContinuePromptState() {
    const bool saveLoaded = m_saveManager.loadFromFile(
        SaveManager::existingSavePath());

    if (saveLoaded) {
        const SaveData& saveData = m_saveManager.getSaveData();
        m_savedMapPath = saveData.mapPath;

        const std::string catalogPath = normalizedPath(saveData.mapPath);
        if (const auto* level = LevelCatalog::findByMapPath(catalogPath)) {
            m_savedWorldName = level->worldName;
            m_savedHudWorldName = level->hudWorldName;
        } else {
            const std::string levelName =
                std::filesystem::path(saveData.mapPath).stem().string();
            if (!levelName.empty()) {
                m_savedWorldName = "WORLD " + levelName;
                m_savedHudWorldName = levelName;
            }
        }

        m_saveAvailable = pathExists(saveData.mapPath)
            && pathExists(saveData.tilesetPath)
            && !m_savedWorldName.empty()
            && !m_savedHudWorldName.empty();
    }

    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "[ContinuePromptState] Failed to load menu font.\n";
    }

    m_fadeOverlay.setSize({1280.f, 720.f});
    m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, 175));

    m_panel.setSize({650.f, 370.f});
    m_panel.setOrigin(325.f, 185.f);
    m_panel.setPosition(640.f, 360.f);
    m_panel.setFillColor(sf::Color(14, 29, 56, 245));
    m_panel.setOutlineColor(sf::Color(235, 235, 235));
    m_panel.setOutlineThickness(4.f);

    m_titleText.setFont(m_font);
    m_titleText.setString("SAVED PROGRESS");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color(255, 220, 55));
    m_titleText.setOutlineColor(sf::Color(170, 30, 25));
    m_titleText.setOutlineThickness(4.f);
    const sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                          titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(640.f, 235.f);

    m_promptText.setFont(m_font);
    m_promptText.setString(m_saveAvailable
        ? "DO YOU WANT TO CONTINUE\nYOUR SAVED PROGRESS?"
        : "SAVED PROGRESS IS INVALID");
    m_promptText.setCharacterSize(22);
    m_promptText.setFillColor(sf::Color::White);
    m_promptText.setLineSpacing(1.3f);
    const sf::FloatRect promptBounds = m_promptText.getLocalBounds();
    m_promptText.setOrigin(promptBounds.left + promptBounds.width / 2.f,
                           promptBounds.top + promptBounds.height / 2.f);
    m_promptText.setPosition(640.f, 315.f);

    const std::array<const char*, 2> labels = {"CONTINUE", "NEW GAME"};
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        PromptButton& button = m_buttons[i];
        button.shape.setSize({250.f, 58.f});
        button.shape.setOrigin(125.f, 29.f);
        button.shape.setPosition(
            490.f + static_cast<float>(i) * 300.f, 440.f);
        button.shape.setOutlineThickness(3.f);

        button.label.setFont(m_font);
        button.label.setString(labels[i]);
        button.label.setCharacterSize(23);
        const sf::FloatRect bounds = button.label.getLocalBounds();
        button.label.setOrigin(bounds.left + bounds.width / 2.f,
                               bounds.top + bounds.height / 2.f);
        button.label.setPosition(button.shape.getPosition());
    }

    m_buttons[0].enabled = m_saveAvailable;
    m_selectedButton = m_saveAvailable ? 0 : 1;
    updateButtonAppearance();
}

void ContinuePromptState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        const int hovered = buttonAt({
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        });
        if (hovered != m_hoveredButton) {
            m_hoveredButton = hovered;
            if (hovered >= 0) m_selectedButton = hovered;
            updateButtonAppearance();
        }
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        const int clicked = buttonAt({
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        });
        if (clicked >= 0) {
            m_selectedButton = clicked;
            m_hoveredButton = clicked;
            updateButtonAppearance();
            activateButton(clicked);
        }
        return;
    }

    if (event.type != sf::Event::KeyPressed) return;

    if (event.key.code == sf::Keyboard::Escape) {
        Game::getInstance().popState();
        return;
    }
    if (event.key.code == sf::Keyboard::Left
        || event.key.code == sf::Keyboard::Up) {
        moveSelection(-1);
        return;
    }
    if (event.key.code == sf::Keyboard::Right
        || event.key.code == sf::Keyboard::Down) {
        moveSelection(1);
        return;
    }
    if (event.key.code == sf::Keyboard::Enter) {
        activateButton(m_selectedButton);
    }
}

void ContinuePromptState::update(sf::Time) {
}

void ContinuePromptState::render(sf::RenderWindow& window) {
    window.setView(window.getDefaultView());
    window.draw(m_fadeOverlay);
    window.draw(m_panel);
    window.draw(m_titleText);
    window.draw(m_promptText);
    for (const PromptButton& button : m_buttons) {
        window.draw(button.shape);
        window.draw(button.label);
    }
}

void ContinuePromptState::selectButton(int index) {
    if (index < 0 || index >= static_cast<int>(m_buttons.size())
        || !m_buttons[static_cast<std::size_t>(index)].enabled) {
        return;
    }
    m_selectedButton = index;
    m_hoveredButton = -1;
    updateButtonAppearance();
}

void ContinuePromptState::moveSelection(int direction) {
    const int buttonCount = static_cast<int>(m_buttons.size());
    int candidate = m_selectedButton;
    for (int checked = 0; checked < buttonCount; ++checked) {
        candidate = (candidate + direction + buttonCount) % buttonCount;
        if (m_buttons[static_cast<std::size_t>(candidate)].enabled) {
            selectButton(candidate);
            return;
        }
    }
}

void ContinuePromptState::activateButton(int index) {
    if (index < 0 || index >= static_cast<int>(m_buttons.size())
        || !m_buttons[static_cast<std::size_t>(index)].enabled) {
        return;
    }
    if (index == 0) {
        continueSavedProgress();
    } else {
        startNewGame();
    }
}

void ContinuePromptState::updateButtonAppearance() {
    const int activeButton = m_hoveredButton >= 0
        ? m_hoveredButton : m_selectedButton;
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        PromptButton& button = m_buttons[i];
        if (!button.enabled) {
            button.shape.setFillColor(sf::Color(45, 50, 60, 210));
            button.shape.setOutlineColor(sf::Color(105, 110, 120, 210));
            button.label.setFillColor(sf::Color(125, 130, 140));
            continue;
        }

        const bool selected = static_cast<int>(i) == activeButton;
        button.shape.setFillColor(selected
            ? sf::Color(245, 195, 45, 235)
            : sf::Color(20, 35, 65, 230));
        button.shape.setOutlineColor(selected
            ? sf::Color::White
            : sf::Color(230, 230, 230, 220));
        button.label.setFillColor(selected
            ? sf::Color(125, 25, 20)
            : sf::Color::White);
    }
}

int ContinuePromptState::buttonAt(sf::Vector2f point) const {
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i].enabled
            && m_buttons[i].shape.getGlobalBounds().contains(point)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void ContinuePromptState::continueSavedProgress() {
    const SaveData& saveData = m_saveManager.getSaveData();
    Game& game = Game::getInstance();
    game.setSelectedHero(parseHeroType(saveData.hero.heroType));
    game.setSelectedLevel(m_savedMapPath, m_savedWorldName);

    auto hud = std::make_shared<HUDManager>();
    hud->init("assets/fonts/SuperMario256.ttf");
    hud->restoreProgress(saveData.hud.score,
                         saveData.hud.coins,
                         saveData.hud.lives);
    hud->resetTimer(saveData.hud.remainingTime);
    hud->setWorld(m_savedHudWorldName);

    game.clearStatesAndChange(std::make_unique<TransitionState>(
        std::move(hud), m_savedMapPath, m_savedWorldName, true));
}

void ContinuePromptState::startNewGame() {
    Game::getInstance().clearStatesAndChange(
        std::make_unique<TransitionState>());
}
