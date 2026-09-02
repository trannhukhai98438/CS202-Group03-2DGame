#include "Core/MainMenuState.h"
#include "Core/ContinuePromptState.h"
#include "Core/Game.h"
#include "Core/PlayingState.h"
#include "Core/TransitionState.h"
#include "Core/CharacterSelectState.h"
#include "Core/GuideState.h"
#include "Core/LevelSelectState.h"
#include "Core/SettingsState.h"
#include "Managers/SaveManager.hpp"
#include <array>
#include <filesystem>
#include <iostream>

MainMenuState::MainMenuState() {
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "Error loading font\n";
	}
	if (!m_backgroundTexture.loadFromFile("assets/textures/menuBackground.png")) {
		std::cerr << "Error loading menu background\n";
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);
	const sf::Vector2u textureSize = m_backgroundTexture.getSize();
	if (textureSize.x > 0 && textureSize.y > 0) {
		m_backgroundSprite.setScale(1280.f / textureSize.x,
		                            720.f / textureSize.y);
	}

	m_overlay.setSize({1280.f, 720.f});
	m_overlay.setFillColor(sf::Color(0, 0, 0, 70));

	m_titleText.setFont(m_font);
	m_titleText.setString("SUPER MARIO");
	m_titleText.setCharacterSize(72);
	m_titleText.setFillColor(sf::Color(255, 220, 55));
	m_titleText.setOutlineColor(sf::Color(170, 30, 25));
	m_titleText.setOutlineThickness(5.f);
	const sf::FloatRect titleBounds = m_titleText.getLocalBounds();
	m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
	                      titleBounds.top + titleBounds.height / 2.f);
	m_titleText.setPosition(640.f, 135.f);

	const std::array<std::string, 5> labels = {
		"PLAY", "CHARACTER", "LEVEL", "GUIDE", "SETTINGS"
	};
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		auto& button = m_buttons[i];
		button.shape.setSize({330.f, 54.f});
		button.shape.setOrigin(165.f, 27.f);
		button.shape.setPosition(640.f, 245.f + static_cast<float>(i) * 68.f);
		button.shape.setOutlineThickness(3.f);

		button.label.setFont(m_font);
		button.label.setString(labels[i]);
		button.label.setCharacterSize(24);
		const sf::FloatRect bounds = button.label.getLocalBounds();
		button.label.setOrigin(bounds.left + bounds.width / 2.f,
		                       bounds.top + bounds.height / 2.f);
		button.label.setPosition(button.shape.getPosition());
	}
	updateButtonAppearance();

	m_statusText.setFont(m_font);
	m_statusText.setString(
		"SELECTED: " + Game::getInstance().getSelectedWorldName());
	m_statusText.setCharacterSize(16);
	m_statusText.setFillColor(sf::Color(235, 235, 235));
	const sf::FloatRect statusBounds = m_statusText.getLocalBounds();
	m_statusText.setOrigin(
		statusBounds.left + statusBounds.width / 2.f,
		statusBounds.top + statusBounds.height / 2.f);
	m_statusText.setPosition(640.f, 605.f);

	Game::getInstance().getSoundManager().playBGM("name_entry", true);
}

void MainMenuState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::MouseMoved) {
		const int hovered = buttonAt({
			static_cast<float>(event.mouseMove.x),
			static_cast<float>(event.mouseMove.y)
		});
		if (hovered != m_hoveredButton) {
			m_hoveredButton = hovered;
			if (hovered >= 0) {
				m_selectedButton = hovered;
			}
			updateButtonAppearance();
		}
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed
		&& event.mouseButton.button == sf::Mouse::Left) {
		const int clicked = buttonAt({static_cast<float>(event.mouseButton.x),
		                              static_cast<float>(event.mouseButton.y)});
		if (clicked >= 0) {
			m_selectedButton = clicked;
			m_hoveredButton = clicked;
			updateButtonAppearance();
			activateButton(clicked);
		}
		return;
	}

	if (event.type != sf::Event::KeyPressed) return;

	const int buttonCount = static_cast<int>(m_buttons.size());
	const int activeButton = m_hoveredButton >= 0
		? m_hoveredButton : m_selectedButton;
	if (event.key.code == sf::Keyboard::Up) {
		selectButton((activeButton + buttonCount - 1) % buttonCount);
		return;
	}
	if (event.key.code == sf::Keyboard::Down) {
		selectButton((activeButton + 1) % buttonCount);
		return;
	}
	if (event.key.code == sf::Keyboard::Enter) {
		selectButton(activeButton);
		activateButton(activeButton);
	}
}

void MainMenuState::update(sf::Time dt) {
	// For now: empty because menu has no moving parts.
}

void MainMenuState::render(sf::RenderWindow& window) {
	window.draw(m_backgroundSprite);
	window.draw(m_overlay);
	window.draw(m_titleText);
	for (const auto& button : m_buttons) {
		window.draw(button.shape);
		window.draw(button.label);
	}
	window.draw(m_statusText);
}

int MainMenuState::buttonAt(sf::Vector2f point) const {
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		if (m_buttons[i].shape.getGlobalBounds().contains(point)) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

void MainMenuState::selectButton(int index) {
	if (index < 0 || index >= static_cast<int>(m_buttons.size())) return;
	const bool appearanceChanged = index != m_selectedButton
		|| m_hoveredButton >= 0;
	m_selectedButton = index;
	m_hoveredButton = -1;
	if (appearanceChanged) updateButtonAppearance();
}

void MainMenuState::activateButton(int index) {
	if (index == 0) {
		std::error_code error;
		const std::filesystem::path savePath =
			SaveManager::existingSavePath();
		const bool hasSave = std::filesystem::is_regular_file(
			savePath, error) && !error;
		if (hasSave) {
			Game::getInstance().pushState(
				std::make_unique<ContinuePromptState>());
		} else {
			Game::getInstance().changeState(
				std::make_unique<TransitionState>());
		}
		return;
	}
	if (index == 1) {
		Game::getInstance().changeState(
			std::make_unique<CharacterSelectState>());
		return;
	}
	if (index == 3) {
		Game::getInstance().changeState(std::make_unique<GuideState>());
		return;
	}
	if (index == 4) {
		Game::getInstance().changeState(std::make_unique<SettingsState>());
		return;
	}
	if (index == 2) {
		Game::getInstance().changeState(
			std::make_unique<LevelSelectState>());
		return;
	}
}

void MainMenuState::updateButtonAppearance() {
	const int activeButton = m_hoveredButton >= 0
		? m_hoveredButton : m_selectedButton;
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		const bool selected = static_cast<int>(i) == activeButton;
		auto& button = m_buttons[i];
		button.shape.setFillColor(selected
			? sf::Color(245, 195, 45, 235)
			: sf::Color(20, 35, 65, 210));
		button.shape.setOutlineColor(selected
			? sf::Color::White
			: sf::Color(230, 230, 230, 220));
		button.label.setFillColor(selected
			? sf::Color(125, 25, 20)
			: sf::Color::White);
	}
}
